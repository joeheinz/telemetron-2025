#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QProgressBar>
#include <QFont>
#include <QProcess>
#include <QTimer>
#include <QPixmap>
#include <QScrollArea>
#include <QFile>

class FourierApp : public QWidget
{
    Q_OBJECT

public:
    FourierApp(QWidget *parent = nullptr) : QWidget(parent)
    {
        setWindowTitle("SC206E Fourier Camera Analyzer");
        setFixedSize(800, 600);
        
        // Configurar fuente básica embebida
        setupEmbeddedFont();
        setupUI();
        setupConnections();
    }

private slots:
    void onTakePhoto()
    {
        logOutput->append("Iniciando captura de foto...");
        progressBar->setVisible(true);
        progressBar->setValue(30);
        
        // Ejecutar tomar_foto() del script Python
        QProcess *process = new QProcess(this);
        process->setProgram("python3");
        process->setArguments({"/data/fourier.py", "--action", "photo"});
        
        connect(process, QOverload<int>::of(&QProcess::finished), [this, process](int exitCode) {
            progressBar->setValue(100);
            if (exitCode == 0) {
                logOutput->append("Foto capturada exitosamente");
                enableAnalysisButtons(true);
            } else {
                logOutput->append("Error al capturar foto");
            }
            process->deleteLater();
            QTimer::singleShot(1000, [this]() { progressBar->setVisible(false); });
        });
        
        process->start();
    }
    
    void onAnalyze1D()
    {
        logOutput->append("Iniciando analisis FFT 1D...");
        progressBar->setVisible(true);
        progressBar->setValue(50);
        
        QProcess *process = new QProcess(this);
        process->setProgram("python3");
        process->setArguments({"/data/fourier.py", "--action", "fft1d"});
        
        connect(process, QOverload<int>::of(&QProcess::finished), [this, process](int exitCode) {
            progressBar->setValue(100);
            if (exitCode == 0) {
                logOutput->append("Analisis FFT 1D completado");
                logOutput->append("Grafico guardado en /data/misc/camera/fourier_plot.png");
                showResultButton->setVisible(true);
            } else {
                logOutput->append("Error en analisis FFT 1D");
            }
            process->deleteLater();
            QTimer::singleShot(1000, [this]() { progressBar->setVisible(false); });
        });
        
        process->start();
    }
    
    void onAnalyze2D()
    {
        logOutput->append("Iniciando analisis FFT 2D...");
        progressBar->setVisible(true);
        progressBar->setValue(50);
        
        QProcess *process = new QProcess(this);
        process->setProgram("python3");
        process->setArguments({"/data/fourier.py", "--action", "fft2d"});
        
        connect(process, QOverload<int>::of(&QProcess::finished), [this, process](int exitCode) {
            progressBar->setValue(100);
            if (exitCode == 0) {
                logOutput->append("Analisis FFT 2D completado");
                logOutput->append("Grafico guardado en /data/misc/camera/fourier_2d_plot.png");
                showResultButton->setVisible(true);
            } else {
                logOutput->append("Error en analisis FFT 2D");
            }
            process->deleteLater();
            QTimer::singleShot(1000, [this]() { progressBar->setVisible(false); });
        });
        
        process->start();
    }
    
    void onShowResults()
    {
        logOutput->append("Mostrando resultados...");
        
        // Mostrar imagen con weston-image en background
        QProcess *westonProcess = new QProcess(this);
        westonProcess->setProgram("weston-image");
        
        // Verificar si existe gráfico 2D primero, sino 1D
        if (QFile::exists("/data/misc/camera/fourier_2d_plot.png")) {
            westonProcess->setArguments({"/data/misc/camera/fourier_2d_plot.png"});
        } else {
            westonProcess->setArguments({"/data/misc/camera/fourier_plot.png"});
        }
        
        westonProcess->start();
        
        // Cerrar weston-image después de 10 segundos
        QTimer::singleShot(10000, [westonProcess]() {
            if (westonProcess->state() != QProcess::NotRunning) {
                westonProcess->terminate();
                westonProcess->deleteLater();
            }
        });
        
        logOutput->append("Resultado mostrado por 10 segundos");
    }
    
    void onFullWorkflow()
    {
        logOutput->append("Iniciando flujo completo: Foto -> FFT 1D -> FFT 2D");
        
        // Secuencia: foto → esperar → fft1d → esperar → fft2d → mostrar
        onTakePhoto();
        
        QTimer::singleShot(3000, [this]() {
            onAnalyze1D();
            QTimer::singleShot(5000, [this]() {
                onAnalyze2D();
                QTimer::singleShot(5000, [this]() {
                    onShowResults();
                });
            });
        });
    }

private:
    void setupEmbeddedFont()
    {
        // Configurar fuente simple sin depender del sistema
        QFont defaultFont("Monospace", 12, QFont::Bold);
        if (!defaultFont.exactMatch()) {
            // Fallback a cualquier fuente disponible
            defaultFont = QFont("", 14, QFont::Bold);
        }
        
        // Aplicar fuente a toda la aplicación
        QApplication::setFont(defaultFont);
        
        // Configurar stylesheet global para asegurar visibilidad
        setStyleSheet(
            "QWidget { font-size: 14px; color: black; background-color: white; }"
            "QPushButton { font-size: 16px; color: white; font-weight: bold; }"
            "QLabel { font-size: 18px; color: black; font-weight: bold; }"
            "QTextEdit { font-size: 12px; color: black; background-color: #f0f0f0; }"
        );
    }
    
    void setupUI()
    {
        QVBoxLayout *mainLayout = new QVBoxLayout;
        
        // Título
        QLabel *title = new QLabel("FOURIER CAMERA ANALYZER");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-size: 24px; font-weight: bold; color: black; margin: 10px; background-color: yellow;");
        
        // Botones principales
        QHBoxLayout *mainButtonsLayout = new QHBoxLayout;
        
        takePhotoBtn = new QPushButton("TOMAR FOTO");
        takePhotoBtn->setStyleSheet("QPushButton { background-color: red; color: white; font-size: 20px; padding: 15px; font-weight: bold; }");
        
        analyze1DBtn = new QPushButton("FFT 1D");
        analyze1DBtn->setStyleSheet("QPushButton { background-color: blue; color: white; font-size: 20px; padding: 15px; font-weight: bold; }");
        analyze1DBtn->setEnabled(false);
        
        analyze2DBtn = new QPushButton("FFT 2D");
        analyze2DBtn->setStyleSheet("QPushButton { background-color: green; color: white; font-size: 20px; padding: 15px; font-weight: bold; }");
        analyze2DBtn->setEnabled(false);
        
        mainButtonsLayout->addWidget(takePhotoBtn);
        mainButtonsLayout->addWidget(analyze1DBtn);
        mainButtonsLayout->addWidget(analyze2DBtn);
        
        // Botones secundarios
        QHBoxLayout *secondButtonsLayout = new QHBoxLayout;
        
        showResultButton = new QPushButton("VER RESULTADOS");
        showResultButton->setStyleSheet("QPushButton { background-color: orange; color: black; font-size: 18px; padding: 10px; font-weight: bold; }");
        showResultButton->setVisible(false);
        
        fullWorkflowBtn = new QPushButton("FLUJO COMPLETO");
        fullWorkflowBtn->setStyleSheet("QPushButton { background-color: purple; color: white; font-size: 18px; padding: 10px; font-weight: bold; }");
        
        QPushButton *exitBtn = new QPushButton("SALIR");
        exitBtn->setStyleSheet("QPushButton { background-color: black; color: white; font-size: 18px; padding: 10px; font-weight: bold; }");
        
        secondButtonsLayout->addWidget(showResultButton);
        secondButtonsLayout->addWidget(fullWorkflowBtn);
        secondButtonsLayout->addWidget(exitBtn);
        
        // Progress bar
        progressBar = new QProgressBar;
        progressBar->setStyleSheet("QProgressBar { border: 2px solid #2E86AB; border-radius: 5px; text-align: center; } QProgressBar::chunk { background-color: #F18F01; }");
        progressBar->setVisible(false);
        
        // Log output
        QLabel *logLabel = new QLabel("LOG DE ACTIVIDAD:");
        logLabel->setStyleSheet("font-weight: bold; color: black; font-size: 16px; background-color: cyan;");
        
        logOutput = new QTextEdit;
        logOutput->setStyleSheet("QTextEdit { background-color: white; border: 3px solid black; font-size: 14px; color: black; }");
        logOutput->setMaximumHeight(200);
        logOutput->append("Aplicacion iniciada. Presiona TOMAR FOTO para comenzar.");
        
        // Layout assembly
        mainLayout->addWidget(title);
        mainLayout->addLayout(mainButtonsLayout);
        mainLayout->addLayout(secondButtonsLayout);
        mainLayout->addWidget(progressBar);
        mainLayout->addWidget(logLabel);
        mainLayout->addWidget(logOutput);
        
        setLayout(mainLayout);
        
        // Conectar salir
        connect(exitBtn, &QPushButton::clicked, this, &QWidget::close);
    }
    
    void setupConnections()
    {
        connect(takePhotoBtn, &QPushButton::clicked, this, &FourierApp::onTakePhoto);
        connect(analyze1DBtn, &QPushButton::clicked, this, &FourierApp::onAnalyze1D);
        connect(analyze2DBtn, &QPushButton::clicked, this, &FourierApp::onAnalyze2D);
        connect(showResultButton, &QPushButton::clicked, this, &FourierApp::onShowResults);
        connect(fullWorkflowBtn, &QPushButton::clicked, this, &FourierApp::onFullWorkflow);
    }
    
    void enableAnalysisButtons(bool enabled)
    {
        analyze1DBtn->setEnabled(enabled);
        analyze2DBtn->setEnabled(enabled);
    }

private:
    QPushButton *takePhotoBtn;
    QPushButton *analyze1DBtn;
    QPushButton *analyze2DBtn;
    QPushButton *showResultButton;
    QPushButton *fullWorkflowBtn;
    QProgressBar *progressBar;
    QTextEdit *logOutput;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    FourierApp window;
    window.show();
    
    return app.exec();
}

#include "main_fourier.moc"