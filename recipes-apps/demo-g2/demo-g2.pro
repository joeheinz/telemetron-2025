QT += core widgets gui

TARGET = demo-g2-viewer
TEMPLATE = app

SOURCES += image_viewer.cpp

# Configuración para cross-compilation
target.path = /usr/bin
INSTALLS += target