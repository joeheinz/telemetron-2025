#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QDir>
#include <QStringList>
#include <QPainter>
#include <QScrollArea>

class ImageViewer : public QWidget
{
    Q_OBJECT

public:
    ImageViewer(QWidget *parent = nullptr) : QWidget(parent), currentIndex(0)
    {
        setFixedSize(800, 600);
        setStyleSheet("background-color: white;");
        
        loadImageList();
        setupUI();
        
        if (!imageFiles.isEmpty()) {
            showImage(0);
        }
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.fillRect(rect(), Qt::white);
        
        // Título
        painter.setPen(QPen(Qt::black, 3));
        QFont titleFont;
        titleFont.setPixelSize(24);
        titleFont.setBold(true);
        painter.setFont(titleFont);
        painter.drawText(50, 30, 700, 40, Qt::AlignCenter, "IMAGE VIEWER");
        
        // Estado actual
        if (!imageFiles.isEmpty()) {
            QFont statusFont;
            statusFont.setPixelSize(16);
            painter.setFont(statusFont);
            
            QString status = QString("Imagen %1 de %2: %3")
                           .arg(currentIndex + 1)
                           .arg(imageFiles.size())
                           .arg(imageFiles[currentIndex]);
            
            painter.drawText(50, 550, 700, 30, Qt::AlignCenter, status);
        } else {
            QFont errorFont;
            errorFont.setPixelSize(18);
            painter.setFont(errorFont);
            painter.setPen(QPen(Qt::red, 3));
            painter.drawText(50, 300, 700, 40, Qt::AlignCenter, "NO SE ENCONTRARON IMAGENES");
        }
    }

private slots:
    void showPreviousImage()
    {
        if (imageFiles.isEmpty()) return;
        
        currentIndex--;
        if (currentIndex < 0) {
            currentIndex = imageFiles.size() - 1;
        }
        showImage(currentIndex);
        update();
    }
    
    void showNextImage()
    {
        if (imageFiles.isEmpty()) return;
        
        currentIndex++;
        if (currentIndex >= imageFiles.size()) {
            currentIndex = 0;
        }
        showImage(currentIndex);
        update();
    }

private:
    void loadImageList()
    {
        QDir imageDir("/data/demo/graficos/images");
        QStringList filters;
        filters << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp" << "*.gif";
        
        imageFiles = imageDir.entryList(filters, QDir::Files);
        
        // Si no hay imágenes, crear algunas de prueba
        if (imageFiles.isEmpty()) {
            imageFiles << "test1.png" << "test2.jpg" << "sample.png";
        }
    }
    
    void showImage(int index)
    {
        if (index < 0 || index >= imageFiles.size()) return;
        
        QString imagePath = "/data/demo/graficos/images/" + imageFiles[index];
        QPixmap pixmap(imagePath);
        
        if (pixmap.isNull()) {
            // Si no se puede cargar la imagen, mostrar un rectángulo con el nombre
            pixmap = QPixmap(400, 300);
            pixmap.fill(Qt::lightGray);
            
            QPainter painter(&pixmap);
            painter.setPen(Qt::black);
            QFont font;
            font.setPixelSize(16);
            painter.setFont(font);
            painter.drawText(pixmap.rect(), Qt::AlignCenter, 
                           QString("Imagen:\n%1\n\n(No se pudo cargar)").arg(imageFiles[index]));
        }
        
        // Escalar la imagen para que quepa en el área de visualización
        QPixmap scaledPixmap = pixmap.scaled(500, 350, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel->setPixmap(scaledPixmap);
        imageLabel->setAlignment(Qt::AlignCenter);
    }
    
    void setupUI()
    {
        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->setSpacing(20);
        mainLayout->setContentsMargins(50, 80, 50, 80);
        
        // Área de imagen
        imageLabel = new QLabel;
        imageLabel->setFixedSize(500, 350);
        imageLabel->setStyleSheet("border: 2px solid black; background-color: #f0f0f0;");
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setText("Cargando imagen...");
        
        // Centrar la imagen
        QHBoxLayout *imageLayout = new QHBoxLayout;
        imageLayout->addStretch();
        imageLayout->addWidget(imageLabel);
        imageLayout->addStretch();
        
        // Botones de navegación
        QHBoxLayout *buttonLayout = new QHBoxLayout;
        
        QPushButton *prevBtn = new QPushButton("ANTERIOR");
        prevBtn->setFixedSize(150, 50);
        prevBtn->setStyleSheet("background-color: blue; color: white; font-size: 16px; font-weight: bold;");
        connect(prevBtn, &QPushButton::clicked, this, &ImageViewer::showPreviousImage);
        
        QPushButton *nextBtn = new QPushButton("SIGUIENTE");
        nextBtn->setFixedSize(150, 50);
        nextBtn->setStyleSheet("background-color: green; color: white; font-size: 16px; font-weight: bold;");
        connect(nextBtn, &QPushButton::clicked, this, &ImageViewer::showNextImage);
        
        QPushButton *exitBtn = new QPushButton("SALIR");
        exitBtn->setFixedSize(100, 50);
        exitBtn->setStyleSheet("background-color: red; color: white; font-size: 16px; font-weight: bold;");
        connect(exitBtn, &QPushButton::clicked, this, &QWidget::close);
        
        buttonLayout->addWidget(prevBtn);
        buttonLayout->addStretch();
        buttonLayout->addWidget(nextBtn);
        buttonLayout->addStretch();
        buttonLayout->addWidget(exitBtn);
        
        // Layout principal
        mainLayout->addLayout(imageLayout);
        mainLayout->addLayout(buttonLayout);
        
        setLayout(mainLayout);
    }

private:
    QStringList imageFiles;
    int currentIndex;
    QLabel *imageLabel;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    ImageViewer viewer;
    viewer.show();
    
    return app.exec();
}

#include "image_viewer.moc"