#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QFont>

class SimpleApp : public QWidget
{
public:
    SimpleApp(QWidget *parent = nullptr) : QWidget(parent)
    {
        setWindowTitle("SC206E Qt5 App Simple");
        setFixedSize(800, 600);
        
        // Layout principal
        QVBoxLayout *mainLayout = new QVBoxLayout;
        
        // Título
        QLabel *title = new QLabel("¡Hola SC206E!");
        title->setAlignment(Qt::AlignCenter);
        QFont titleFont;
        titleFont.setPointSize(24);
        titleFont.setBold(true);
        title->setFont(titleFont);
        
        // Botones
        QHBoxLayout *buttonLayout = new QHBoxLayout;
        QPushButton *btn1 = new QPushButton("Botón 1");
        QPushButton *btn2 = new QPushButton("Botón 2");
        QPushButton *btn3 = new QPushButton("Salir");
        
        buttonLayout->addWidget(btn1);
        buttonLayout->addWidget(btn2);
        buttonLayout->addWidget(btn3);
        
        // Area de texto
        QTextEdit *textArea = new QTextEdit;
        textArea->setText("Esta es una aplicación Qt5 simple corriendo en SC206E!\n\n"
                         "✅ Qt5 funcionando\n"
                         "✅ EGLFS activo\n" 
                         "✅ Pantalla completa\n\n"
                         "Presiona los botones para probar...");
        
        // Conectar botones
        connect(btn1, &QPushButton::clicked, [textArea]() {
            textArea->append("✅ Botón 1 presionado!");
        });
        
        connect(btn2, &QPushButton::clicked, [textArea]() {
            textArea->append("✅ Botón 2 presionado!");
        });
        
        connect(btn3, &QPushButton::clicked, [this]() {
            close();
        });
        
        // Agregar al layout principal
        mainLayout->addWidget(title);
        mainLayout->addLayout(buttonLayout);
        mainLayout->addWidget(textArea);
        
        setLayout(mainLayout);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    SimpleApp window;
    window.show();
    
    return app.exec();
}