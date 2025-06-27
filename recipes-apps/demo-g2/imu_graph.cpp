#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QProcess>
#include <QTimer>
#include <QPixmap>
#include <QTextStream>
#include <QDebug>
#include <QList>
#include <QFont>
#include <cmath>

class IMUGraphWidget : public QWidget
{
    Q_OBJECT

public:
    IMUGraphWidget(QWidget *parent = nullptr) : QWidget(parent), isCapturing(false), sampleCount(0)
    {
        setFixedSize(800, 600);
        setStyleSheet("background-color: white;");
        
        // Configurar buffers para datos
        xData.clear();
        yData.clear();
        zData.clear();
        maxSamples = 150;
        
        setupUI();
        setupProcess();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.fillRect(rect(), Qt::white);
        
        // === HEADER AREA (Logo + Título) ===
        QRect headerRect(0, 0, 800, 100);
        painter.fillRect(headerRect, QColor(240, 240, 240));
        painter.setPen(QPen(Qt::black, 2));
        painter.drawRect(headerRect);
        
        // Dibujar logo más grande
        if (!logo.isNull()) {
            QPixmap scaledLogo = logo.scaled(140, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            int logoX = 20;
            int logoY = (100 - scaledLogo.height()) / 2;
            painter.drawPixmap(logoX, logoY, scaledLogo);
        }
        
        // Título al lado del logo
        painter.setPen(QPen(Qt::black, 3));
        QFont titleFont;
        titleFont.setPixelSize(22);
        titleFont.setBold(true);
        painter.setFont(titleFont);
        painter.drawText(180, 15, 600, 30, Qt::AlignLeft, "ACELEROMETRO EN TIEMPO REAL");
        
        // Estado debajo del título
        QFont statusFont;
        statusFont.setPixelSize(14);
        painter.setFont(statusFont);
        QString status = QString("Muestras: %1  |  Estado: %2")
                        .arg(sampleCount)
                        .arg(isCapturing ? "CAPTURANDO" : "DETENIDO");
        painter.setPen(QPen(isCapturing ? Qt::darkGreen : Qt::red, 2));
        painter.drawText(180, 50, 600, 20, Qt::AlignLeft, status);
        
        // === GRAPH AREA ===
        drawGraph(painter);
        
        // === VALUES AREA ===
        QRect valuesRect(0, 500, 800, 50);
        painter.fillRect(valuesRect, QColor(250, 250, 250));
        painter.setPen(QPen(Qt::black, 1));
        painter.drawRect(valuesRect);
        
        if (!xData.isEmpty()) {
            double x = xData.last();
            double y = yData.last();
            double z = zData.last();
            double magnitude = std::sqrt(x*x + y*y + z*z);
            
            QFont valueFont;
            valueFont.setPixelSize(16);
            valueFont.setBold(true);
            painter.setFont(valueFont);
            
            // Valores actuales con colores
            QString xVal = QString("X: %1").arg(x, 0, 'f', 2);
            QString yVal = QString("Y: %1").arg(y, 0, 'f', 2);
            QString zVal = QString("Z: %1").arg(z, 0, 'f', 2);
            QString magVal = QString("|G|: %1").arg(magnitude, 0, 'f', 2);
            
            painter.setPen(QPen(Qt::red, 2));
            painter.drawText(50, 515, 150, 30, Qt::AlignCenter, xVal);
            
            painter.setPen(QPen(Qt::green, 2));
            painter.drawText(200, 515, 150, 30, Qt::AlignCenter, yVal);
            
            painter.setPen(QPen(Qt::blue, 2));
            painter.drawText(350, 515, 150, 30, Qt::AlignCenter, zVal);
            
            painter.setPen(QPen(Qt::black, 2));
            painter.drawText(500, 515, 150, 30, Qt::AlignCenter, magVal);
        }
    }

private slots:
    void startCapture()
    {
        if (isCapturing) return;
        
        isCapturing = true;
        sampleCount = 0;
        
        // Limpiar datos anteriores
        xData.clear();
        yData.clear();
        zData.clear();
        
        // Iniciar proceso see_workhorse
        seeProcess->start("see_workhorse", QStringList() 
                         << "-debug=1" 
                         << "-display_events=1" 
                         << "-sensor=accel" 
                         << "-sample_rate=50" 
                         << "-duration=3600"); // 1 hora máximo
        
        startBtn->setEnabled(false);
        stopBtn->setEnabled(true);
        
        update();
    }
    
    void stopCapture()
    {
        if (!isCapturing) return;
        
        isCapturing = false;
        
        if (seeProcess->state() != QProcess::NotRunning) {
            seeProcess->terminate();
            if (!seeProcess->waitForFinished(3000)) {
                seeProcess->kill();
            }
        }
        
        startBtn->setEnabled(true);
        stopBtn->setEnabled(false);
        
        update();
    }
    
    void processData()
    {
        QByteArray data = seeProcess->readAllStandardOutput();
        QString output = QString::fromUtf8(data);
        
        // Procesar línea por línea
        QStringList lines = output.split('\n');
        for (const QString &line : lines) {
            if (line.contains("\"data\"")) {
                currentDataBlock = line;
                capturing = true;
            } else if (capturing && line.contains("]")) {
                currentDataBlock += line;
                parseDataBlock();
                capturing = false;
                currentDataBlock.clear();
            } else if (capturing) {
                currentDataBlock += line;
            }
        }
    }
    
    void parseDataBlock()
    {
        try {
            int start = currentDataBlock.indexOf("[");
            int end = currentDataBlock.indexOf("]");
            
            if (start == -1 || end == -1) return;
            
            QString dataStr = currentDataBlock.mid(start + 1, end - start - 1);
            QStringList valueStrs = dataStr.split(",");
            
            if (valueStrs.size() >= 3) {
                double x = valueStrs[0].trimmed().toDouble();
                double y = valueStrs[1].trimmed().toDouble();
                double z = valueStrs[2].trimmed().toDouble();
                
                // Agregar a buffers
                xData.append(x);
                yData.append(y);
                zData.append(z);
                
                // Mantener tamaño máximo
                while (xData.size() > maxSamples) {
                    xData.removeFirst();
                    yData.removeFirst();
                    zData.removeFirst();
                }
                
                sampleCount++;
                
                // Actualizar gráfica
                update();
                
                // Verificar magnitud alta
                double magnitude = std::sqrt(x*x + y*y + z*z);
                if (magnitude > 15.0) {
                    qDebug() << "Magnitud alta detectada:" << magnitude;
                }
            }
        } catch (...) {
            qDebug() << "Error parseando datos:" << currentDataBlock;
        }
    }

private:
    void setupUI()
    {
        // Cargar logo
        logo.load("/data/demo/graficos/images/logo.png");
        
        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->setSpacing(5);
        mainLayout->setContentsMargins(0, 0, 0, 10);
        
        // Espaciador para empujar botones hacia abajo (después del área de valores)
        QSpacerItem *spacer = new QSpacerItem(20, 550, QSizePolicy::Minimum, QSizePolicy::Fixed);
        mainLayout->addItem(spacer);
        
        // Botones de control en la parte inferior
        QHBoxLayout *buttonLayout = new QHBoxLayout;
        buttonLayout->setSpacing(15);
        buttonLayout->setContentsMargins(30, 5, 30, 5);
        
        startBtn = new QPushButton("START");
        startBtn->setFixedSize(140, 45);
        startBtn->setStyleSheet(
            "QPushButton { "
            "background-color: #28a745; color: white; font-size: 16px; font-weight: bold; "
            "border: 2px solid #1e7e34; border-radius: 8px; "
            "} "
            "QPushButton:hover { background-color: #34ce57; } "
            "QPushButton:pressed { background-color: #1e7e34; }"
        );
        connect(startBtn, &QPushButton::clicked, this, &IMUGraphWidget::startCapture);
        
        stopBtn = new QPushButton("STOP");
        stopBtn->setFixedSize(140, 45);
        stopBtn->setStyleSheet(
            "QPushButton { "
            "background-color: #dc3545; color: white; font-size: 16px; font-weight: bold; "
            "border: 2px solid #c82333; border-radius: 8px; "
            "} "
            "QPushButton:hover { background-color: #e4606d; } "
            "QPushButton:pressed { background-color: #c82333; } "
            "QPushButton:disabled { background-color: #6c757d; border-color: #6c757d; }"
        );
        stopBtn->setEnabled(false);
        connect(stopBtn, &QPushButton::clicked, this, &IMUGraphWidget::stopCapture);
        
        QPushButton *exitBtn = new QPushButton("SALIR");
        exitBtn->setFixedSize(100, 45);
        exitBtn->setStyleSheet(
            "QPushButton { "
            "background-color: #6c757d; color: white; font-size: 14px; font-weight: bold; "
            "border: 2px solid #545b62; border-radius: 8px; "
            "} "
            "QPushButton:hover { background-color: #5a6268; } "
            "QPushButton:pressed { background-color: #545b62; }"
        );
        connect(exitBtn, &QPushButton::clicked, this, &QWidget::close);
        
        buttonLayout->addWidget(startBtn);
        buttonLayout->addWidget(stopBtn);
        buttonLayout->addStretch();
        buttonLayout->addWidget(exitBtn);
        
        mainLayout->addLayout(buttonLayout);
        
        setLayout(mainLayout);
    }
    
    void setupProcess()
    {
        seeProcess = new QProcess(this);
        connect(seeProcess, &QProcess::readyReadStandardOutput, this, &IMUGraphWidget::processData);
        connect(seeProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [this](int exitCode, QProcess::ExitStatus) {
            qDebug() << "Proceso see_workhorse terminado con código:" << exitCode;
            stopCapture();
        });
    }
    
    void drawGraph(QPainter &painter)
    {
        // Área de gráfica bien definida (sin traslaparse)
        QRect graphRect(30, 110, 740, 320);
        
        // Fondo de gráfica
        painter.fillRect(graphRect, Qt::white);
        painter.setPen(QPen(Qt::black, 2));
        painter.drawRect(graphRect);
        
        // Grid lines
        painter.setPen(QPen(Qt::lightGray, 1, Qt::DashLine));
        int centerY = graphRect.center().y();
        
        // Líneas horizontales (cada 50 pixels)
        for (int y = graphRect.top() + 40; y < graphRect.bottom(); y += 40) {
            painter.drawLine(graphRect.left(), y, graphRect.right(), y);
        }
        
        // Línea central Y=0 más marcada
        painter.setPen(QPen(Qt::gray, 2, Qt::DashLine));
        painter.drawLine(graphRect.left(), centerY, graphRect.right(), centerY);
        
        // Líneas verticales (cada 60 pixels)
        painter.setPen(QPen(Qt::lightGray, 1, Qt::DashLine));
        for (int x = graphRect.left() + 60; x < graphRect.right(); x += 60) {
            painter.drawLine(x, graphRect.top(), x, graphRect.bottom());
        }
        
        // Etiquetas de escala
        painter.setPen(QPen(Qt::black, 1));
        QFont scaleFont;
        scaleFont.setPixelSize(10);
        painter.setFont(scaleFont);
        
        // Etiquetas Y (g-force)
        painter.drawText(5, centerY - 80, "+4g");
        painter.drawText(5, centerY - 40, "+2g");
        painter.drawText(5, centerY + 5, " 0g");
        painter.drawText(5, centerY + 45, "-2g");
        painter.drawText(5, centerY + 85, "-4g");
        
        // Dibujar datos si existen
        if (xData.size() > 1) {
            double xStep = (double)graphRect.width() / (maxSamples - 1);
            
            // Dibujar X (rojo)
            painter.setPen(QPen(Qt::red, 3));
            for (int i = 1; i < xData.size(); i++) {
                double x1 = graphRect.left() + (i-1) * xStep;
                double y1 = centerY - xData[i-1] * 20; // Escalar: 20 pixels = 1g
                double x2 = graphRect.left() + i * xStep;
                double y2 = centerY - xData[i] * 20;
                
                // Limitar dentro del área de gráfica
                y1 = qMax((double)graphRect.top(), qMin((double)graphRect.bottom(), y1));
                y2 = qMax((double)graphRect.top(), qMin((double)graphRect.bottom(), y2));
                
                painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
            }
            
            // Dibujar Y (verde)
            painter.setPen(QPen(Qt::green, 3));
            for (int i = 1; i < yData.size(); i++) {
                double x1 = graphRect.left() + (i-1) * xStep;
                double y1 = centerY - yData[i-1] * 20;
                double x2 = graphRect.left() + i * xStep;
                double y2 = centerY - yData[i] * 20;
                
                y1 = qMax((double)graphRect.top(), qMin((double)graphRect.bottom(), y1));
                y2 = qMax((double)graphRect.top(), qMin((double)graphRect.bottom(), y2));
                
                painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
            }
            
            // Dibujar Z (azul)
            painter.setPen(QPen(Qt::blue, 3));
            for (int i = 1; i < zData.size(); i++) {
                double x1 = graphRect.left() + (i-1) * xStep;
                double y1 = centerY - zData[i-1] * 20;
                double x2 = graphRect.left() + i * xStep;
                double y2 = centerY - zData[i] * 20;
                
                y1 = qMax((double)graphRect.top(), qMin((double)graphRect.bottom(), y1));
                y2 = qMax((double)graphRect.top(), qMin((double)graphRect.bottom(), y2));
                
                painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
            }
        }
        
        // Leyenda en esquina superior derecha
        QRect legendRect(650, 120, 110, 80);
        painter.fillRect(legendRect, QColor(255, 255, 255, 200));
        painter.setPen(QPen(Qt::black, 1));
        painter.drawRect(legendRect);
        
        QFont legendFont;
        legendFont.setPixelSize(12);
        legendFont.setBold(true);
        painter.setFont(legendFont);
        
        painter.setPen(QPen(Qt::red, 3));
        painter.drawLine(660, 135, 680, 135);
        painter.setPen(QPen(Qt::black, 2));
        painter.drawText(685, 140, "X");
        
        painter.setPen(QPen(Qt::green, 3));
        painter.drawLine(660, 155, 680, 155);
        painter.setPen(QPen(Qt::black, 2));
        painter.drawText(685, 160, "Y");
        
        painter.setPen(QPen(Qt::blue, 3));
        painter.drawLine(660, 175, 680, 175);
        painter.setPen(QPen(Qt::black, 2));
        painter.drawText(685, 180, "Z");
        
        // Título de gráfica
        painter.setPen(QPen(Qt::black, 2));
        QFont graphTitleFont;
        graphTitleFont.setPixelSize(14);
        graphTitleFont.setBold(true);
        painter.setFont(graphTitleFont);
        painter.drawText(graphRect.left(), graphRect.top() - 10, graphRect.width(), 20, 
                        Qt::AlignCenter, "Aceleracion (g) vs Tiempo");
    }

private:
    QPushButton *startBtn;
    QPushButton *stopBtn;
    QProcess *seeProcess;
    QPixmap logo;
    
    bool isCapturing;
    bool capturing;
    int sampleCount;
    int maxSamples;
    QString currentDataBlock;
    
    QList<double> xData;
    QList<double> yData;
    QList<double> zData;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    IMUGraphWidget viewer;
    viewer.show();
    
    return app.exec();
}

#include "imu_graph.moc"