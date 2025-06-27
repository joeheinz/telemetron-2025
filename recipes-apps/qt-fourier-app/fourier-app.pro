QT += core widgets

TARGET = fourier-qt-app
TEMPLATE = app

SOURCES += main_fourier.cpp

# Configuración para cross-compilation
target.path = /usr/bin
INSTALLS += target