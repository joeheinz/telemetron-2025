# Qt Development Workspace para SC206E

Este directorio contiene aplicaciones Qt desarrolladas para el SC206E.

## Estructura:
- `qt-fourier-app/` - Aplicación de análisis de Fourier con cámara
- `qt-dev-workspace/` - Workspace general para desarrollo Qt

## Apps desarrolladas:

### 1. Fourier Camera App
- Captura de cámara con gstreamer
- Análisis FFT 1D y 2D
- Visualización con Qt/matplotlib
- Menú interactivo

## Cómo usar el SDK:

### 1. Después que termine `bitbake meta-toolchain-qt5`:
```bash
cd build-qti-distro-rb-debug/tmp-glibc/deploy/sdk
./rb-debug-x86_64-meta-toolchain-qt5-aarch64-qrbx210-rbx-toolchain-*.sh
```

### 2. Activar ambiente de desarrollo:
```bash
cd /usr/local/rb-debug-x86_64
source environment-setup-aarch64-oe-linux
```

### 3. Compilar aplicación Qt:
```bash
cd meta-moeller/recipes-apps/qt-dev-workspace/
qmake your-app.pro
make
```

### 4. Transferir al dispositivo:
```bash
adb push your-app /data/
adb shell
export QT_QPA_PLATFORM=eglfs
cd /data
./your-app
```

## Variables de entorno para el dispositivo:
```bash
export QT_QPA_PLATFORM=eglfs
export XDG_RUNTIME_DIR=/run/user/root
export QT_QPA_FONTDIR=/usr/share/fonts
export QML2_IMPORT_PATH=/usr/lib/qml
```

## Próximas aplicaciones:
- [ ] Qt Fourier App (camera + FFT analysis)
- [ ] Sensor dashboard Qt app
- [ ] Video streaming Qt app
- [ ] IoT control panel Qt app