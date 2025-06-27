#!/bin/bash

echo "🔧 Creando directorio de fonts para Qt..."

# Crear directorio que Qt está buscando
mkdir -p /usr/lib/fonts

# Extraer Liberation Fonts directamente
cd /tmp
tar -xzf /home/telemetron/Desktop/Quectel/test/qcm2290_linux_r60_r004-SC206E_rl/downloads/downloads/liberation-fonts-ttf-2.00.1.tar.gz

# Copiar fonts al directorio que Qt busca
cp liberation-fonts-ttf-*/Liberation*.ttf /usr/lib/fonts/

echo "✅ Fonts copiados a /usr/lib/fonts/"
ls -la /usr/lib/fonts/

echo ""
echo "🎯 Ahora ejecuta:"
echo "adb push fix_fonts.sh /data/"
echo "adb shell"
echo "cd /data && chmod +x fix_fonts.sh && ./fix_fonts.sh"
echo "export QT_QPA_PLATFORM=eglfs"
echo "./minimal-app"