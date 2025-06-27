# Demo G2 - Aplicaciones Qt5 para SC206E

Aplicaciones Qt5 desarrolladas específicamente para el dispositivo embebido SC206E (QRB5165/QCM2290) con sistema operativo Yocto Linux.

## 📋 Tabla de Contenidos

- [Descripción General](#descripción-general)
- [Aplicaciones Incluidas](#aplicaciones-incluidas)
- [Requisitos del Sistema](#requisitos-del-sistema)
- [Configuración del Entorno](#configuración-del-entorno)
- [Compilación Paso a Paso](#compilación-paso-a-paso)
- [Instalación en SC206E](#instalación-en-sc206e)
- [Ejecución de Aplicaciones](#ejecución-de-aplicaciones)
- [Troubleshooting](#troubleshooting)
- [Desarrollo Avanzado](#desarrollo-avanzado)

---

## 📖 Descripción General

Este directorio contiene dos aplicaciones Qt5 funcionales que demuestran diferentes capacidades del dispositivo SC206E:

### 🎯 **Objetivos del Proyecto**
- ✅ Resolver problemas de renderizado de texto en Qt5/EGLFS
- ✅ Implementar gráficas en tiempo real con datos de sensores
- ✅ Crear interfaces de usuario intuitivas para dispositivos embebidos
- ✅ Integrar hardware (IMU, cámara) con aplicaciones Qt

### 🏗️ **Arquitectura del Sistema**
```
┌─────────────────────────────────────┐
│         Aplicaciones Qt5            │
│  ┌─────────────┐ ┌─────────────────┐│
│  │ Image Viewer│ │  IMU Graph      ││
│  │             │ │  Real-time      ││
│  └─────────────┘ └─────────────────┘│
├─────────────────────────────────────┤
│           Qt5 Framework             │
│    (Widgets + GUI + Core)           │
├─────────────────────────────────────┤
│              EGLFS                  │
│       (Direct Rendering)            │
├─────────────────────────────────────┤
│           Yocto Linux               │
│         (ARM64/aarch64)             │
├─────────────────────────────────────┤
│            SC206E Device            │
│      QRB5165/QCM2290 SoC           │
└─────────────────────────────────────┘
```

---

## 📱 Aplicaciones Incluidas

### 1. **Image Viewer** (`demo-g2-viewer`)
Navegador de imágenes con interfaz intuitiva.

#### **Características:**
- 🖼️ **Navegación fluida** entre imágenes
- 📁 **Auto-detección** de formatos (JPG, PNG, BMP, GIF)
- 🔍 **Escalado automático** para diferentes resoluciones
- ⏮️⏭️ **Controles simples** (Anterior/Siguiente/Salir)

#### **Funcionalidad:**
- Busca imágenes en `/data/demo/graficos/images/`
- Muestra información de imagen actual (X de Y total)
- Maneja errores de carga con placeholders
- Interfaz optimizada para pantallas embebidas

### 2. **IMU Graph Viewer** (`imu-graph-viewer`)
Monitor de acelerómetro en tiempo real con gráfica profesional.

#### **Características:**
- 📊 **Gráfica en tiempo real** de aceleración (X, Y, Z)
- 🎛️ **Controles START/STOP** para captura
- 🏢 **Logo corporativo** en header
- 📈 **Grid profesional** con escalas (-4g a +4g)
- 📱 **Layout organizado** en secciones bien definidas
- 🔢 **Valores numéricos** actualizados en tiempo real

#### **Integración Hardware:**
- Utiliza comando `see_workhorse` para acceso al IMU
- Parser en tiempo real de datos JSON del acelerómetro
- Buffer circular para mantener historial de 150 muestras
- Detección automática de movimientos bruscos

---

## 🔧 Requisitos del Sistema

### **En el Host (Computadora de Desarrollo):**
- Ubuntu 18.04+ o distribución Linux compatible
- Yocto build system configurado
- Qt5 cross-compilation toolchain instalado
- Acceso ADB al dispositivo SC206E

### **En el SC206E:**
- Sistema operativo Yocto Linux compilado
- Qt5 runtime libraries instaladas
- EGLFS platform plugin configurado
- Comando `see_workhorse` disponible (para IMU app)

### **Archivos Requeridos:**
```
meta-moeller/recipes-apps/demo-g2/
├── image_viewer.cpp         # Código visor de imágenes
├── demo-g2.pro             # Configuración qmake para visor
├── imu_graph.cpp           # Código monitor IMU  
├── imu-graph.pro           # Configuración qmake para IMU
├── LiberationSans-Regular.ttf  # Font principal
├── Vera.ttf                # Font alternativo
└── README.md               # Este archivo
```

---

## ⚙️ Configuración del Entorno

### **Paso 1: Verificar Toolchain Qt5**
```bash
# Verificar que el toolchain esté instalado
ls -la /usr/local/rb-debug-x86_64/

# Debería existir:
# - environment-setup-aarch64-oe-linux
# - sysroots/aarch64-oe-linux/usr/lib/libQt5*.so
# - sysroots/x86_64-qtisdk-linux/usr/bin/qmake
```

### **Paso 2: Activar Entorno de Cross-Compilation**
```bash
cd /usr/local/rb-debug-x86_64
source environment-setup-aarch64-oe-linux

# Verificar configuración
echo $CC                    # Debe mostrar: aarch64-oe-linux-gcc
echo $CXX                   # Debe mostrar: aarch64-oe-linux-g++
which qmake                 # Debe mostrar ruta al qmake cross-compiler
```

### **Paso 3: Configurar Variables de Entorno**
```bash
# Variables críticas para cross-compilation
export CROSS_COMPILE=aarch64-oe-linux-
export PKG_CONFIG_PATH=/usr/local/rb-debug-x86_64/sysroots/aarch64-oe-linux/usr/lib/pkgconfig
export QT_SELECT=5

# Verificar Qt
qmake -v
# Debe mostrar: Using Qt version 5.x.x in /usr/local/rb-debug-x86_64/sysroots/aarch64-oe-linux/usr/lib
```

---

## 🔨 Compilación Paso a Paso

### **Configuración Inicial**
```bash
# Navegar al directorio del proyecto
cd /path/to/qcm2290_linux_r60_r004-SC206E_rl/build-qti-distro-rb-debug/meta-moeller/recipes-apps/demo-g2

# Activar toolchain (CRÍTICO)
source /usr/local/rb-debug-x86_64/environment-setup-aarch64-oe-linux
```

### **Compilar Image Viewer**
```bash
# Limpiar compilaciones anteriores
rm -f demo-g2-viewer Makefile moc_* *.o

# Generar Makefile con qmake
qmake demo-g2.pro

# Verificar que se generó correctamente
ls -la Makefile

# Compilar aplicación
make clean && make

# Verificar ejecutable
ls -la demo-g2-viewer
file demo-g2-viewer    # Debe mostrar: ARM aarch64, dynamically linked
```

### **Compilar IMU Graph Viewer**
```bash
# Limpiar compilaciones anteriores  
rm -f imu-graph-viewer Makefile moc_* *.o

# Generar Makefile
qmake imu-graph.pro

# Compilar
make clean && make

# Verificar ejecutable
ls -la imu-graph-viewer
file imu-graph-viewer  # Debe mostrar: ARM aarch64, dynamically linked
```

### **Verificar Dependencias**
```bash
# Ver librerías Qt requeridas
aarch64-oe-linux-objdump -p demo-g2-viewer | grep NEEDED
aarch64-oe-linux-objdump -p imu-graph-viewer | grep NEEDED

# Debe mostrar:
# libQt5Widgets.so.5
# libQt5Gui.so.5  
# libQt5Core.so.5
# libGLESv2.so.2
# libc.so.6
```

### **Solución de Problemas de Compilación**
```bash
# Error: qmake: command not found
source /usr/local/rb-debug-x86_64/environment-setup-aarch64-oe-linux

# Error: Qt headers not found
export PKG_CONFIG_PATH=/usr/local/rb-debug-x86_64/sysroots/aarch64-oe-linux/usr/lib/pkgconfig

# Error: undefined reference to Qt symbols
# Verificar que se está usando el cross-compiler correcto
echo $CXX    # Debe ser: aarch64-oe-linux-g++

# Error: wrong ELF class ELFCLASS64
# Indica que se compiló para x86, no ARM. Re-ejecutar source environment-setup
```

---

## 📲 Instalación en SC206E

### **Paso 1: Preparar Fonts (CRÍTICO)**

El problema más común es que Qt no puede encontrar fonts. **ESTE PASO ES OBLIGATORIO**.

```bash
# Copiar fonts a directorio local (desde el host)
cp /path/to/fonts/LiberationSans-Regular.ttf .
cp /path/to/fonts/Vera.ttf .

# Transferir fonts al dispositivo
adb push LiberationSans-Regular.ttf /data/
adb push Vera.ttf /data/

# Configurar directorio de fonts en el dispositivo
adb shell << 'EOF'
mkdir -p /data/fonts
cp /data/*.ttf /data/fonts/
ls -la /data/fonts/    # Verificar que se copiaron
EOF
```

### **Paso 2: Transferir Aplicaciones**
```bash
# Transferir ejecutables al dispositivo
adb push demo-g2-viewer /data/
adb push imu-graph-viewer /data/

# Hacer ejecutables
adb shell << 'EOF'
chmod +x /data/demo-g2-viewer
chmod +x /data/imu-graph-viewer
ls -la /data/*viewer    # Verificar permisos
EOF
```

### **Paso 3: Preparar Directorio de Imágenes (Para Image Viewer)**
```bash
# Crear estructura de directorios
adb shell << 'EOF'
mkdir -p /data/demo/graficos/images
EOF

# Transferir imágenes de ejemplo (opcional)
adb push sample1.jpg /data/demo/graficos/images/
adb push sample2.png /data/demo/graficos/images/
adb push logo.png /data/demo/graficos/images/
```

### **Paso 4: Verificar Dependencias en el Dispositivo**
```bash
adb shell << 'EOF'
# Verificar librerías Qt disponibles
ls -la /usr/lib/libQt5*

# Verificar see_workhorse (para IMU app)
which see_workhorse

# Verificar espacio disponible
df -h /data
EOF
```

---

## 🚀 Ejecución de Aplicaciones

### **Configuración Básica de Qt (SIEMPRE REQUERIDA)**
```bash
adb shell
cd /data

# Variables de entorno CRÍTICAS
export QT_QPA_PLATFORM=eglfs
export QT_QPA_FONTDIR=/data/fonts

# Variables opcionales para debugging
export QT_LOGGING_RULES="*.debug=false"
export QT_QPA_EGLFS_DEBUG=0
```

### **Ejecutar Image Viewer**
```bash
# Comando completo
adb shell << 'EOF'
cd /data
export QT_QPA_PLATFORM=eglfs
export QT_QPA_FONTDIR=/data/fonts
./demo-g2-viewer
EOF

# Lo que debería ver:
# - Ventana con título "IMAGE VIEWER"
# - Área central mostrando imagen actual
# - Botones: ANTERIOR, SIGUIENTE, SALIR
# - Contador "Imagen X de Y: filename.jpg"
```

### **Ejecutar IMU Graph Viewer**
```bash
# Comando completo
adb shell << 'EOF'
cd /data  
export QT_QPA_PLATFORM=eglfs
export QT_QPA_FONTDIR=/data/fonts
./imu-graph-viewer
EOF

# Lo que debería ver:
# - Header con logo y título "ACELEROMETRO EN TIEMPO REAL"
# - Área de gráfica con grid y escalas
# - Valores numéricos en tiempo real (X, Y, Z, |G|)
# - Botones: START (verde), STOP (rojo), SALIR (gris)
```

### **Controles de las Aplicaciones**

#### **Image Viewer:**
- **ANTERIOR**: Imagen previa (circular)
- **SIGUIENTE**: Siguiente imagen (circular)  
- **SALIR**: Cerrar aplicación

#### **IMU Graph Viewer:**
- **START**: Iniciar captura de datos del acelerómetro
- **STOP**: Detener captura
- **SALIR**: Cerrar aplicación

### **Comandos de Terminación**
```bash
# Si la aplicación se cuelga, terminar desde otra terminal
adb shell
pkill -f demo-g2-viewer
pkill -f imu-graph-viewer

# O reiniciar completamente
adb reboot
```

---

## 🐛 Troubleshooting

### **Problema 1: Texto no visible / Pantalla blanca**
```
Error: QFontDatabase: Cannot find font directory /usr/lib/fonts
```

**Solución:**
```bash
# Verificar fonts
adb shell ls -la /data/fonts/

# Si no existen, copiar de nuevo
adb push LiberationSans-Regular.ttf /data/
adb shell "mkdir -p /data/fonts && cp /data/*.ttf /data/fonts/"

# Verificar variable de entorno
adb shell
export QT_QPA_FONTDIR=/data/fonts
echo $QT_QPA_FONTDIR    # Debe mostrar: /data/fonts
```

### **Problema 2: Aplicación no arranca**
```
Error: Cannot open display
```

**Solución:**
```bash
# Verificar plataforma
export QT_QPA_PLATFORM=eglfs    # NO wayland, NO xcb

# Verificar que no hay otra aplicación Qt corriendo
pkill -f qt
pkill -f demo-g2
pkill -f imu-graph

# Verificar permisos
adb shell
chmod +x /data/demo-g2-viewer
chmod +x /data/imu-graph-viewer
```

### **Problema 3: IMU no funciona**
```
Error: see_workhorse not found
```

**Solución:**
```bash
# Verificar see_workhorse
adb shell which see_workhorse

# Si no existe, verificar que el paquete se instaló
adb shell
find /usr -name "*see*" -type f

# Probar manualmente
see_workhorse -sensor=accel -sample_rate=10 -duration=5
```

### **Problema 4: Imágenes no cargan**
```
Mensaje: "No se encontraron imágenes"
```

**Solución:**
```bash
# Verificar directorio
adb shell ls -la /data/demo/graficos/images/

# Si no existe, crear estructura
adb shell "mkdir -p /data/demo/graficos/images"

# Transferir imágenes de prueba
adb push *.jpg /data/demo/graficos/images/
adb push *.png /data/demo/graficos/images/
```

### **Problema 5: Aplicación se cierra inmediatamente**
```bash
# Ejecutar con debug para ver errores
adb shell << 'EOF'
export QT_QPA_PLATFORM=eglfs
export QT_QPA_FONTDIR=/data/fonts
export QT_LOGGING_RULES="*.debug=true"
cd /data
./demo-g2-viewer 2>&1 | tee debug.log
EOF

# Ver log de errores
adb shell cat /data/debug.log
```

### **Scripts de Diagnóstico**
```bash
# Script completo de verificación
adb shell << 'EOF'
echo "=== DIAGNÓSTICO SISTEMA ==="
echo "1. Verificando Qt libraries:"
ls -la /usr/lib/libQt5* | head -5

echo "2. Verificando fonts:"
ls -la /data/fonts/

echo "3. Verificando aplicaciones:"
ls -la /data/*viewer

echo "4. Verificando see_workhorse:"
which see_workhorse

echo "5. Verificando espacio en disco:"
df -h /data

echo "6. Verificando procesos Qt:"
ps aux | grep qt

echo "=== FIN DIAGNÓSTICO ==="
EOF
```

---

## 🔬 Desarrollo Avanzado

### **Estructura del Código**

#### **Image Viewer (image_viewer.cpp)**
```cpp
class ImageViewer : public QWidget
{
    Q_OBJECT

private:
    QStringList imageFiles;      // Lista de archivos de imagen
    int currentIndex;            // Índice de imagen actual
    QLabel *imageLabel;          // Widget para mostrar imagen
    
    // Métodos principales
    void loadImageList();        // Cargar lista de archivos
    void showImage(int index);   // Mostrar imagen específica
    void setupUI();              // Configurar interfaz
    
private slots:
    void showPreviousImage();    // Imagen anterior
    void showNextImage();        // Siguiente imagen
};
```

#### **IMU Graph Viewer (imu_graph.cpp)**
```cpp
class IMUGraphWidget : public QWidget
{
    Q_OBJECT

private:
    // Datos del acelerómetro
    QList<double> xData, yData, zData;
    int maxSamples = 150;        // Buffer circular
    int sampleCount;             // Contador de muestras
    
    // Proceso para see_workhorse
    QProcess *seeProcess;
    bool isCapturing;
    QString currentDataBlock;    // Buffer para parsing
    
    // UI Components
    QPushButton *startBtn, *stopBtn;
    QPixmap logo;
    
    // Métodos principales
    void setupUI();              // Configurar interfaz
    void setupProcess();         // Configurar QProcess
    void drawGraph(QPainter &);  // Dibujar gráfica
    void parseDataBlock();       // Parser de datos JSON
    
private slots:
    void startCapture();         // Iniciar captura
    void stopCapture();          // Detener captura
    void processData();          // Procesar datos en tiempo real
};
```

### **Protocolo de Datos IMU**
```bash
# Comando see_workhorse
see_workhorse -debug=1 -display_events=1 -sensor=accel -sample_rate=50 -duration=3600

# Formato de salida (JSON multilinea)
{
  "timestamp": 1234567890,
  "sensor": "accel",
  "data": [
    1.23,    # X acceleration (g)
    -0.45,   # Y acceleration (g)  
    9.81     # Z acceleration (g)
  ]
}
```

### **Parser de Datos en Tiempo Real**
```cpp
void IMUGraphWidget::processData()
{
    QByteArray data = seeProcess->readAllStandardOutput();
    QString output = QString::fromUtf8(data);
    
    QStringList lines = output.split('\n');
    for (const QString &line : lines) {
        if (line.contains("\"data\"")) {
            currentDataBlock = line;
            capturing = true;
        } else if (capturing && line.contains("]")) {
            currentDataBlock += line;
            parseDataBlock();           // Procesar bloque completo
            capturing = false;
            currentDataBlock.clear();
        } else if (capturing) {
            currentDataBlock += line;   // Acumular líneas
        }
    }
}

void IMUGraphWidget::parseDataBlock()
{
    try {
        // Buscar array JSON: [x.xx, y.yy, z.zz]
        int start = currentDataBlock.indexOf("[");
        int end = currentDataBlock.indexOf("]");
        
        QString dataStr = currentDataBlock.mid(start + 1, end - start - 1);
        QStringList valueStrs = dataStr.split(",");
        
        if (valueStrs.size() >= 3) {
            double x = valueStrs[0].trimmed().toDouble();
            double y = valueStrs[1].trimmed().toDouble();
            double z = valueStrs[2].trimmed().toDouble();
            
            // Agregar a buffers circulares
            xData.append(x); yData.append(y); zData.append(z);
            
            // Mantener tamaño máximo
            while (xData.size() > maxSamples) {
                xData.removeFirst();
                yData.removeFirst(); 
                zData.removeFirst();
            }
            
            sampleCount++;
            update();  // Trigger repaint
        }
    } catch (...) {
        qDebug() << "Error parsing:" << currentDataBlock;
    }
}
```

### **Renderizado de Gráfica**
```cpp
void IMUGraphWidget::drawGraph(QPainter &painter)
{
    // Área de gráfica definida
    QRect graphRect(30, 110, 740, 320);
    
    // Fondo y bordes
    painter.fillRect(graphRect, Qt::white);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawRect(graphRect);
    
    // Grid horizontal cada 40 pixels (escalas de 2g)
    painter.setPen(QPen(Qt::lightGray, 1, Qt::DashLine));
    for (int y = graphRect.top() + 40; y < graphRect.bottom(); y += 40) {
        painter.drawLine(graphRect.left(), y, graphRect.right(), y);
    }
    
    // Línea central Y=0
    int centerY = graphRect.center().y();
    painter.setPen(QPen(Qt::gray, 2, Qt::DashLine));
    painter.drawLine(graphRect.left(), centerY, graphRect.right(), centerY);
    
    // Dibujar datos si existen
    if (xData.size() > 1) {
        double xStep = (double)graphRect.width() / (maxSamples - 1);
        
        // Dibujar canal X (rojo)
        painter.setPen(QPen(Qt::red, 3));
        for (int i = 1; i < xData.size(); i++) {
            double x1 = graphRect.left() + (i-1) * xStep;
            double y1 = centerY - xData[i-1] * 20;  // 20 pixels = 1g
            double x2 = graphRect.left() + i * xStep;
            double y2 = centerY - xData[i] * 20;
            
            // Clamp dentro del área
            y1 = qMax((double)graphRect.top(), qMin((double)graphRect.bottom(), y1));
            y2 = qMax((double)graphRect.top(), qMin((double)graphRect.bottom(), y2));
            
            painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
        }
        
        // Repetir para Y (verde) y Z (azul)...
    }
    
    // Leyenda en esquina superior derecha
    QRect legendRect(650, 120, 110, 80);
    painter.fillRect(legendRect, QColor(255, 255, 255, 200));
    
    // Dibujar líneas de ejemplo + etiquetas
    painter.setPen(QPen(Qt::red, 3));
    painter.drawLine(660, 135, 680, 135);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawText(685, 140, "X");
    // ... Y, Z
}
```

### **Configuraciones qmake**

#### **demo-g2.pro**
```pro
QT += core widgets gui        # Módulos Qt requeridos

TARGET = demo-g2-viewer       # Nombre del ejecutable
TEMPLATE = app               # Tipo: aplicación

SOURCES += image_viewer.cpp   # Archivo fuente principal

# Configuración para cross-compilation
target.path = /usr/bin       # Ruta de instalación en target
INSTALLS += target           # Incluir en make install
```

#### **imu-graph.pro** 
```pro
QT += core widgets gui        # Módulos Qt requeridos

TARGET = imu-graph-viewer     # Nombre del ejecutable  
TEMPLATE = app               # Tipo: aplicación

SOURCES += imu_graph.cpp      # Archivo fuente principal

# Configuración para cross-compilation
target.path = /usr/bin       # Ruta de instalación en target
INSTALLS += target           # Incluir en make install
```

### **Optimizaciones de Performance**

#### **Buffer Circular para Datos**
```cpp
// Mantener solo últimas N muestras en memoria
while (xData.size() > maxSamples) {
    xData.removeFirst();  // O(1) para QList
    yData.removeFirst();
    zData.removeFirst();
}
```

#### **Repaint Selectivo**
```cpp
// Solo repintar cuando hay nuevos datos
void processData() {
    if (parseNewData()) {
        update();  // Trigger paintEvent solo si hay cambios
    }
}
```

#### **Clipping de Datos**
```cpp
// Limitar datos dentro del área visible
y1 = qMax((double)graphRect.top(), qMin((double)graphRect.bottom(), y1));
y2 = qMax((double)graphRect.top(), qMin((double)graphRect.bottom(), y2));
```

---

## 📄 Archivos de Configuración

### **Scripts de Automatización**

#### **compile_all.sh**
```bash
#!/bin/bash
set -e

echo "🔧 Compilando aplicaciones Demo G2..."

# Activar toolchain
source /usr/local/rb-debug-x86_64/environment-setup-aarch64-oe-linux

# Compilar Image Viewer
echo "📱 Compilando Image Viewer..."
qmake demo-g2.pro
make clean && make
echo "✅ Image Viewer compilado: demo-g2-viewer"

# Compilar IMU Graph Viewer  
echo "📊 Compilando IMU Graph Viewer..."
qmake imu-graph.pro
make clean && make
echo "✅ IMU Graph Viewer compilado: imu-graph-viewer"

# Verificar ejecutables
echo "🔍 Verificando ejecutables:"
file demo-g2-viewer imu-graph-viewer

echo "🎉 Compilación completa!"
```

#### **deploy_to_device.sh**
```bash
#!/bin/bash
set -e

echo "📲 Desplegando aplicaciones a SC206E..."

# Transferir fonts (si no existen)
echo "🔤 Configurando fonts..."
adb push LiberationSans-Regular.ttf /data/ 2>/dev/null || true
adb push Vera.ttf /data/ 2>/dev/null || true

adb shell << 'EOF'
mkdir -p /data/fonts
cp /data/*.ttf /data/fonts/ 2>/dev/null || true
EOF

# Transferir aplicaciones
echo "📱 Transferir aplicaciones..."
adb push demo-g2-viewer /data/
adb push imu-graph-viewer /data/

# Configurar permisos
adb shell << 'EOF'
chmod +x /data/demo-g2-viewer
chmod +x /data/imu-graph-viewer

# Crear directorios necesarios
mkdir -p /data/demo/graficos/images
EOF

echo "✅ Despliegue completo!"
echo ""
echo "🚀 Para ejecutar:"
echo "   Image Viewer:    adb shell 'cd /data && export QT_QPA_PLATFORM=eglfs && export QT_QPA_FONTDIR=/data/fonts && ./demo-g2-viewer'"
echo "   IMU Graph:       adb shell 'cd /data && export QT_QPA_PLATFORM=eglfs && export QT_QPA_FONTDIR=/data/fonts && ./imu-graph-viewer'"
```

#### **run_image_viewer.sh**
```bash
#!/bin/bash
adb shell << 'EOF'
cd /data
export QT_QPA_PLATFORM=eglfs
export QT_QPA_FONTDIR=/data/fonts
./demo-g2-viewer
EOF
```

#### **run_imu_graph.sh**
```bash
#!/bin/bash
adb shell << 'EOF'
cd /data  
export QT_QPA_PLATFORM=eglfs
export QT_QPA_FONTDIR=/data/fonts
./imu-graph-viewer
EOF
```

---

## 📚 Referencias y Recursos

### **Documentación Qt5**
- [Qt5 Documentation](https://doc.qt.io/qt-5/)
- [Qt for Embedded Linux](https://doc.qt.io/qt-5/embedded-linux.html)
- [EGLFS Platform Plugin](https://doc.qt.io/qt-5/embedded-linux.html#eglfs)

### **Yocto Project**
- [Yocto Project Documentation](https://www.yoctoproject.org/docs/)
- [Cross-compilation with Qt](https://wiki.yoctoproject.org/wiki/Application_Development_with_Extensible_SDK)

### **SC206E Hardware**
- QRB5165/QCM2290 SoC Documentation
- Sensor Integration Guide
- Display Configuration Manual

### **Comandos Útiles de Referencia**
```bash
# Verificar librerías Qt en target
adb shell ldd /usr/lib/libQt5Core.so

# Ver procesos Qt corriendo
adb shell ps aux | grep qt

# Monitorear uso de CPU/memoria
adb shell top -p $(adb shell pidof demo-g2-viewer)

# Ver logs del kernel relacionados con display
adb shell dmesg | grep -i egl

# Verificar configuración de display
adb shell cat /sys/class/graphics/fb0/modes
```

---

## 🏆 Conclusión

Este proyecto demuestra la implementación exitosa de aplicaciones Qt5 profesionales en dispositivos embebidos con las siguientes contribuciones:

### **🎯 Logros Técnicos:**
1. ✅ **Solución de fonts** - Resolución del problema crítico de renderizado de texto
2. ✅ **Gráficas en tiempo real** - Implementación eficiente de visualización de datos
3. ✅ **Integración hardware** - Comunicación exitosa con sensores IMU
4. ✅ **Interfaz optimizada** - UI adaptada para pantallas embebidas

### **📋 Entregables:**
- **2 aplicaciones Qt funcionales** listas para producción
- **Documentación completa** de desarrollo y deployment
- **Scripts de automatización** para compilación y despliegue
- **Solución robusta de fonts** reutilizable en otros proyectos

### **🔮 Próximos Pasos:**
- Integración con más sensores (giroscopio, magnetómetro)
- Persistencia de datos en base de datos local
- Comunicación wireless (WiFi, Bluetooth)
- Optimizaciones adicionales de performance

---

*Documentación actualizada: Diciembre 2024*  
*Desarrollado para SC206E (QRB5165/QCM2290) con Yocto Linux*  
*Framework: Qt5 + EGLFS + Cross-compilation toolchain*