QT += core widgets gui

TARGET = imu-graph-viewer
TEMPLATE = app

SOURCES += imu_graph.cpp

# Configuración para cross-compilation
target.path = /usr/bin
INSTALLS += target