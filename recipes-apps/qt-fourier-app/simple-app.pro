QT += core widgets

TARGET = simple-qt-app
TEMPLATE = app

SOURCES += main.cpp

# Configuración para cross-compilation
target.path = /usr/bin
INSTALLS += target