QT += core widgets

TARGET = minimal-app
TEMPLATE = app

SOURCES += minimal.cpp

# Configuración para cross-compilation
target.path = /usr/bin
INSTALLS += target