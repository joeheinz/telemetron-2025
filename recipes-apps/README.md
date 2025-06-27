# Qt Applications for SC206E Embedded Device

Este directorio contiene aplicaciones Qt5 desarrolladas para el dispositivo embebido SC206E (QRB5165/QCM2290) utilizando el sistema operativo Yocto Linux.

## 📋 Índice

- [Descripción General](#descripción-general)
- [Aplicaciones Desarrolladas](#aplicaciones-desarrolladas)
- [Solución de Fonts](#solución-de-fonts)
- [Arquitectura del Sistema](#arquitectura-del-sistema)
- [Instalación y Configuración](#instalación-y-configuración)
- [Desarrollo y Compilación](#desarrollo-y-compilación)
- [Documentación Técnica](#documentación-técnica)

---

## 📖 Descripción General

Este proyecto resuelve el desafío de crear aplicaciones Qt5 funcionales en dispositivos embebidos con limitaciones específicas:

- **Sistema operativo**: Yocto Linux con Qt5 + EGLFS
- **Arquitectura**: ARM64 (aarch64)
- **Display**: Sin servidor X11, usando EGLFS para renderizado directo
- **Limitaciones**: Sistema de archivos raíz de solo lectura, problemas de fonts

### 🎯 Objetivos Logrados

1. ✅ **Renderizado de texto** - Solucionado problema crítico de fonts en Qt
2. ✅ **Gráficas en tiempo real** - Implementación de visualización IMU
3. ✅ **Visor de imágenes** - Navegación por galería de imágenes
4. ✅ **Integración hardware** - Comunicación con sensores mediante `see_workhorse`

---

## 📱 Aplicaciones Desarrolladas

### 1. **qt-fourier-app** - Análisis de Fourier con Cámara
**Ubicación**: `qt-fourier-app/`
**Ejecutable**: `fourier-qt-app`

#### Funcionalidades:
- 📸 **Captura de fotos** mediante GStreamer (`qtiqmmfsrc`, `v4l2src`)
- 📊 **Análisis FFT 1D** de líneas horizontales
- 🌈 **Análisis FFT 2D** de imágenes completas
- 🚀 **Flujo automático** foto → FFT 1D → FFT 2D → visualización
- 📈 **Visualización** con `weston-image`

#### Arquitectura:
```cpp
class FourierApp : public QWidget
{
    // Slots principales
    void onTakePhoto()     // Ejecuta Python: fourier.py --action photo
    void onAnalyze1D()     // Ejecuta Python: fourier.py --action fft1d  
    void onAnalyze2D()     // Ejecuta Python: fourier.py --action fft2d
    void onShowResults()   // Muestra resultados con weston-image
    void onFullWorkflow()  // Secuencia completa automatizada
}
```

#### Integración Python:
```cpp
QProcess *process = new QProcess(this);
process->setProgram("python3");
process->setArguments({"/data/fourier.py", "--action", "photo"});
process->start();
```

### 2. **demo-g2** - Visor de Imágenes Interactivo
**Ubicación**: `demo-g2/`
**Ejecutable**: `demo-g2-viewer`

#### Funcionalidades:
- 🖼️ **Navegación** por imágenes en `/data/demo/graficos/images/`
- ⏮️⏭️ **Controles** anterior/siguiente
- 🔍 **Auto-escala** para diferentes tamaños de imagen
- 📁 **Detección automática** de formatos (JPG, PNG, BMP, GIF)

#### Arquitectura:
```cpp
class ImageViewer : public QWidget
{
private:
    QStringList imageFiles;     // Lista de archivos
    int currentIndex;           // Índice actual
    QLabel *imageLabel;         // Widget de visualización
    
    void loadImageList();       // Cargar archivos del directorio
    void showImage(int index);  // Mostrar imagen específica
}
```

### 3. **demo-g2** - Monitor IMU en Tiempo Real
**Ubicación**: `demo-g2/`
**Ejecutable**: `imu-graph-viewer`

#### Funcionalidades:
- 📊 **Gráfica en tiempo real** de acelerómetro (X, Y, Z)
- 🎛️ **Controles START/STOP** para captura
- 🏢 **Logo corporativo** en header
- 📈 **Grid y escalas** profesionales (-4g a +4g)
- 📱 **Layout organizado** en secciones definidas

#### Integración Hardware:
```cpp
// Comando para captura IMU
seeProcess->start("see_workhorse", QStringList() 
                 << "-debug=1" 
                 << "-display_events=1" 
                 << "-sensor=accel" 
                 << "-sample_rate=50" 
                 << "-duration=3600");
```

#### Parser de Datos:
```cpp
void parseDataBlock() {
    // Busca patrón: "data": [x.xx, y.yy, z.zz]
    int start = currentDataBlock.indexOf("[");
    int end = currentDataBlock.indexOf("]");
    QString dataStr = currentDataBlock.mid(start + 1, end - start - 1);
    QStringList valueStrs = dataStr.split(",");
    
    double x = valueStrs[0].trimmed().toDouble();
    double y = valueStrs[1].trimmed().toDouble(); 
    double z = valueStrs[2].trimmed().toDouble();
    
    // Agregar a buffers circulares
    xData.append(x); yData.append(y); zData.append(z);
}
```

#### Renderizado Gráfico:
```cpp
void drawGraph(QPainter &painter) {
    QRect graphRect(30, 110, 740, 320);  // Área definida
    
    // Grid y escalas
    painter.setPen(QPen(Qt::lightGray, 1, Qt::DashLine));
    for (int y = graphRect.top() + 40; y < graphRect.bottom(); y += 40) {
        painter.drawLine(graphRect.left(), y, graphRect.right(), y);
    }
    
    // Dibujar datos con colores
    painter.setPen(QPen(Qt::red, 3));    // X axis
    painter.setPen(QPen(Qt::green, 3));  // Y axis  
    painter.setPen(QPen(Qt::blue, 3));   // Z axis
}
```

---

## 🔧 Solución de Fonts

### Problema Identificado
```
QFontDatabase: Cannot find font directory /usr/lib/fonts.
Note that Qt no longer ships fonts.
```

### Solución Implementada

#### 1. **Diagnóstico**:
- Qt busca fonts en `/usr/lib/fonts` (no existe)
- Fonts están en `/usr/share/fonts/ttf/` (sistema compilado)
- Sistema raíz es de solo lectura

#### 2. **Workaround**:
```bash
# Copiar fonts a ubicación escribible
mkdir -p /data/fonts
cp /usr/share/fonts/ttf/*.ttf /data/fonts/

# Configurar Qt para usar nueva ubicación  
export QT_QPA_FONTDIR=/data/fonts
export QT_QPA_PLATFORM=eglfs
```

#### 3. **Fonts Incluidos**:
- **Liberation**: Sans, Serif, Mono (regular, bold, italic)
- **Bitstream Vera**: Sans, Serif, Mono
- **DejaVu**: Completa colección (via matplotlib)

#### 4. **Código Qt**:
```cpp
// Configuración de fonts embebidos
void setupEmbeddedFont() {
    QFont defaultFont("", 14, QFont::Bold);
    QApplication::setFont(defaultFont);
    
    setStyleSheet(
        "QWidget { font-size: 14px; color: black; }"
        "QPushButton { font-size: 16px; color: white; font-weight: bold; }"
    );
}
```

---

## 🏗️ Arquitectura del Sistema

### Stack Tecnológico
```
┌─────────────────────────────────────┐
│           Aplicaciones Qt5          │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐│
│  │Fourier  │ │ Viewer  │ │   IMU   ││
│  │   App   │ │   App   │ │  Graph  ││  
│  └─────────┘ └─────────┘ └─────────┘│
├─────────────────────────────────────┤
│            Qt5 Framework            │
│        (Widgets + GUI + Core)       │
├─────────────────────────────────────┤
│               EGLFS                 │
│        (OpenGL ES + DRM)            │
├─────────────────────────────────────┤
│           Yocto Linux               │
│        (aarch64 - ARM64)            │
├─────────────────────────────────────┤
│            Hardware                 │
│    SC206E (QRB5165/QCM2290)        │
└─────────────────────────────────────┘
```

### Comunicación Inter-Proceso
```
Qt App ←→ QProcess ←→ Python Scripts
Qt App ←→ QProcess ←→ see_workhorse (IMU)
Qt App ←→ QProcess ←→ GStreamer (Camera)
Qt App ←→ QProcess ←→ weston-image (Display)
```

### Flujo de Datos IMU
```
see_workhorse → stdout → QProcess → parseDataBlock() → 
QList<double> → paintEvent() → QPainter → Display
```

---

## 🚀 Instalación y Configuración

### Prerequisitos
- Yocto build system configurado
- Qt5 toolchain instalado (`meta-toolchain-qt5`)
- Dispositivo SC206E con acceso ADB

### Paso 1: Configurar Toolchain
```bash
cd /usr/local/rb-debug-x86_64
source environment-setup-aarch64-oe-linux
```

### Paso 2: Compilar Aplicaciones
```bash
cd meta-moeller/recipes-apps/qt-fourier-app/
qmake fourier-app.pro && make

cd ../demo-g2/
qmake demo-g2.pro && make
qmake imu-graph.pro && make
```

### Paso 3: Configurar Fonts en Dispositivo
```bash
# Transferir fonts (una sola vez)
adb push LiberationSans-Regular.ttf /data/
adb push Vera.ttf /data/

# Configurar en el dispositivo
adb shell << 'EOF'
mkdir -p /data/fonts
cp /data/*.ttf /data/fonts/
EOF
```

### Paso 4: Ejecutar Aplicaciones
```bash
# Script de configuración
adb shell << 'EOF'
export QT_QPA_PLATFORM=eglfs
export QT_QPA_FONTDIR=/data/fonts
cd /data

# Ejecutar aplicación deseada
./fourier-qt-app        # Análisis Fourier
./demo-g2-viewer        # Visor imágenes  
./imu-graph-viewer      # Monitor IMU
EOF
```

---

## ⚙️ Desarrollo y Compilación

### Estructura de Proyecto
```
meta-moeller/recipes-apps/
├── README.md                    # Este archivo
├── qt-fourier-app/             # Aplicación Fourier
│   ├── main_fourier.cpp        # Código principal
│   ├── fourier-app.pro         # Configuración qmake
│   └── fourier-qt-app          # Ejecutable compilado
└── demo-g2/                    # Aplicaciones demo
    ├── image_viewer.cpp        # Visor de imágenes
    ├── demo-g2.pro             # Configuración qmake
    ├── demo-g2-viewer          # Ejecutable visor
    ├── imu_graph.cpp           # Monitor IMU
    ├── imu-graph.pro           # Configuración qmake
    └── imu-graph-viewer        # Ejecutable IMU
```

### Comandos de Desarrollo
```bash
# Limpiar y recompilar
make clean && make

# Compilar con debug
qmake CONFIG+=debug && make

# Ver dependencias
ldd aplicacion-qt

# Transferir al dispositivo
adb push aplicacion-qt /data/
```

### Configuración qmake
```pro
QT += core widgets gui        # Módulos Qt requeridos
TARGET = nombre-aplicacion    # Nombre del ejecutable
TEMPLATE = app               # Tipo de aplicación
SOURCES += main.cpp          # Archivos fuente

# Cross-compilation
target.path = /usr/bin       # Ruta de instalación
INSTALLS += target           # Instalar automáticamente
```

### Debugging
```bash
# Ver salida de Qt en tiempo real
adb shell
export QT_LOGGING_RULES="*=true"
export QT_QPA_PLATFORM=eglfs
export QT_QPA_FONTDIR=/data/fonts
./aplicacion-qt
```

---

## 📚 Documentación Técnica

### APIs Utilizadas

#### Qt5 Core
```cpp
#include <QApplication>      // Aplicación principal
#include <QWidget>           // Widget base
#include <QVBoxLayout>       // Layout vertical
#include <QHBoxLayout>       // Layout horizontal  
#include <QPushButton>       // Botones
#include <QLabel>            // Etiquetas de texto
#include <QPainter>          // Dibujo personalizado
#include <QProcess>          // Ejecución de procesos
#include <QTimer>            // Temporizadores
#include <QPixmap>           // Manejo de imágenes
```

#### Clases Principales
```cpp
// Clase base para todas las aplicaciones
class BaseApp : public QWidget
{
    Q_OBJECT
    
protected:
    void paintEvent(QPaintEvent *) override;  // Renderizado custom
    void setupUI();                           // Configuración interfaz
    
private slots:
    void onButtonClicked();                   // Manejo de eventos
};
```

### Patrones de Diseño Utilizados

#### 1. **Observer Pattern** (Qt Signals/Slots)
```cpp
connect(button, &QPushButton::clicked, this, &App::onButtonClicked);
connect(process, QOverload<int>::of(&QProcess::finished), 
        [this](int exitCode) { /* handle */ });
```

#### 2. **Command Pattern** (QProcess)
```cpp
void executeCommand(const QString &cmd, const QStringList &args) {
    QProcess *process = new QProcess(this);
    process->start(cmd, args);
}
```

#### 3. **Template Method** (paintEvent)
```cpp
void paintEvent(QPaintEvent *) override {
    drawHeader();      // Template method
    drawContent();     // Template method  
    drawFooter();      // Template method
}
```

### Protocolos de Comunicación

#### IMU Data Protocol
```json
{
  "timestamp": 1234567890,
  "sensor": "accel", 
  "data": [x.xx, y.yy, z.zz]
}
```

#### Camera Pipeline
```bash
qtiqmmfsrc → video/x-raw,NV12 → jpegenc → multifilesink
v4l2src → video/x-raw → jpegenc → multifilesink  
gphoto2 → JPEG file → filesystem
```

### Optimizaciones Implementadas

#### 1. **Circular Buffers**
```cpp
QList<double> xData;  // Buffer circular para datos X
while (xData.size() > maxSamples) {
    xData.removeFirst();  // Mantener tamaño fijo
}
```

#### 2. **Lazy Loading**
```cpp
void showImage(int index) {
    if (index == currentIndex) return;  // Evitar recarga
    
    QPixmap pixmap(imagePath);
    if (pixmap.isNull()) {
        // Fallback a imagen placeholder
    }
}
```

#### 3. **Event-Driven Updates**
```cpp
void processData() {
    // Solo actualizar cuando hay nuevos datos
    QByteArray data = process->readAllStandardOutput();
    if (!data.isEmpty()) {
        parseAndUpdate(data);
        update();  // Trigger repaint
    }
}
```

---

## 🐛 Troubleshooting

### Problemas Comunes

#### 1. **Texto no visible**
```bash
# Verificar fonts
ls -la /data/fonts/
export QT_QPA_FONTDIR=/data/fonts
```

#### 2. **Aplicación no arranca**
```bash
# Verificar plataforma
export QT_QPA_PLATFORM=eglfs
# Verificar permisos
chmod +x /data/aplicacion-qt
```

#### 3. **see_workhorse no funciona**
```bash
# Verificar comando
which see_workhorse
# Probar manualmente
see_workhorse -sensor=accel -sample_rate=10 -duration=5
```

#### 4. **Imágenes no cargan**
```bash
# Verificar directorio
ls -la /data/demo/graficos/images/
# Verificar formatos soportados
file imagen.jpg
```

### Logs y Debugging
```bash
# Ver logs Qt detallados
export QT_LOGGING_RULES="*.debug=true"

# Ver logs de procesos
strace -f -e trace=process ./aplicacion-qt

# Monitorear uso de memoria
top -p $(pidof aplicacion-qt)
```

---

## 📄 Licencia y Créditos

**Desarrollado para**: Dispositivo SC206E (QRB5165/QCM2290)  
**Framework**: Qt5 con Yocto Linux  
**Arquitectura**: ARM64 (aarch64)  
**Autor**: Claude Code Assistant  
**Fecha**: Junio 2025  

### Tecnologías Utilizadas
- **Qt5**: Framework de interfaz gráfica
- **Yocto**: Sistema operativo embebido  
- **EGLFS**: Renderizado sin X11
- **GStreamer**: Pipeline multimedia
- **Python/NumPy**: Procesamiento científico
- **see_workhorse**: Driver de sensores

### Referencias
- [Qt5 Documentation](https://doc.qt.io/qt-5/)
- [Yocto Project](https://www.yoctoproject.org/)
- [EGLFS Platform Plugin](https://doc.qt.io/qt-5/embedded-linux.html)

---

*Documentación actualizada: Junio 2025*  
*Para soporte técnico, consultar logs y troubleshooting section*