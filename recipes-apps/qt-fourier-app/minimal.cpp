#include <QApplication>
#include <QWidget>
#include <QPainter>

class MinimalApp : public QWidget
{
    Q_OBJECT

public:
    MinimalApp(QWidget *parent = nullptr) : QWidget(parent)
    {
        setFixedSize(800, 600);
        setStyleSheet("background-color: white;");
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.fillRect(rect(), Qt::white);
        
        // Solo texto "Hello World" en el centro, sin fonts específicos
        painter.setPen(QPen(Qt::black, 5));
        
        // Usar fuente por defecto del sistema, cualquiera que sea
        QFont font;
        font.setPixelSize(48);
        font.setBold(true);
        painter.setFont(font);
        
        // Dibujar "Hello World" en el centro
        painter.drawText(rect(), Qt::AlignCenter, "Hello World");
    }

};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    MinimalApp window;
    window.show();
    
    return app.exec();
}

#include "minimal.moc"