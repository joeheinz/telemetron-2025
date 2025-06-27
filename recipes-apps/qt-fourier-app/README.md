# Simple Qt5 App para SC206E

## 📱 Aplicación súper simple:

- **Título**: "¡Hola SC206E!"
- **3 botones**: Botón 1, Botón 2, Salir
- **Área de texto**: Muestra mensajes cuando presionas botones
- **Tamaño**: 800x600 píxeles (pantalla completa en EGLFS)

## 🛠️ Cómo compilar:

### Una vez que termine `bitbake meta-toolchain-qt5`:

```bash
# 1. Instalar SDK
cd build-qti-distro-rb-debug/tmp-glibc/deploy/sdk
./rb-debug-x86_64-meta-toolchain-qt5-aarch64-qrbx210-rbx-toolchain-*.sh

# 2. Activar ambiente de desarrollo
cd /usr/local/rb-debug-x86_64
source environment-setup-aarch64-oe-linux

# 3. Compilar la app
cd meta-moeller/recipes-apps/qt-fourier-app/
qmake simple-app.pro
make

# 4. Transferir al dispositivo
adb push simple-qt-app /data/

# 5. Ejecutar en SC206E
adb shell
export QT_QPA_PLATFORM=eglfs
cd /data
./simple-qt-app
```

## 🎯 Lo que hace:

- ✅ Ventana Qt5 básica
- ✅ Botones interactivos
- ✅ Texto dinámico
- ✅ Cierre limpio de aplicación
- ✅ Compatible con EGLFS

¡Súper simple pero funcional! 🚀