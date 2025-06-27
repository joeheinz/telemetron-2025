import subprocess
import math
import os
import time
import signal
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from collections import deque

#ALL THE OTHER IMPORTS IN THE SAME FILE!

# Funciones ficticias - implementar después
def flash_control():
    """Control del flash - prender/apagar"""
    print("\n🔦 Control de Flash")
    print("1. Prender Flash")
    print("2. Apagar Flash")
    print("3. Volver al menú principal")
    
    while True:
        opcion = input("Selecciona (1-3): ").strip()
        
        if opcion == "1":
            print("💡 Encendiendo flash...")
            os.system('echo 15 > /sys/class/leds/led:flash_0/brightness')
            result = os.system('echo 1 > /sys/class/leds/led:switch_0/brightness')
            if result == 0:
                print("✅ Flash encendido")
            else:
                print("❌ Error al encender el flash")
                
        elif opcion == "2":
            print("🌑 Apagando flash...")
            result = os.system('echo 0 > /sys/class/leds/led:switch_0/brightness')
            if result == 0:
                print("✅ Flash apagado")
            else:
                print("❌ Error al apagar el flash")
                
        elif opcion == "3":
            print("🔙 Volviendo al menú principal...")
            break
            
        else:
            print("⚠️ Opción inválida. Por favor selecciona 1-3.")
        
        # Pequeña pausa antes de mostrar el menú de nuevo
        time.sleep(1)

def tomar_foto():
    """Tomar foto y mostrarla"""
    print("\n📸 Tomando foto...")
    
    # Iniciar gstreamer en background
    proc = subprocess.Popen([
        'gst-launch-1.0', '-e', 'qtiqmmfsrc', 'camera=0', 'name=qmmf', '!', 
        'video/x-raw', ',', 'format=NV12,', 'width=640,', 'height=480,', 'framerate=30/1', '!', 
        'jpegenc', '!', 'multifilesink', 'location=/data/misc/camera/snapshot.jpg'
    ])
    
    # Esperar 3 segundos para capturar
    time.sleep(3)
    
    # Terminar el proceso
    proc.terminate()
    proc.wait()
    
    print("📷 Foto capturada, mostrando...")
    
    # Mostrar la imagen
    imagen_proc = subprocess.Popen(['weston-image', '/data/misc/camera/snapshot.jpg'])
    
    # Esperar 10 segundos para ver la imagen
    time.sleep(10)
    
    # Cerrar el visor de imagen
    imagen_proc.terminate()
    imagen_proc.wait()
    
    print("✅ Foto tomada y mostrada")
    print("Presiona Enter para volver al menú...")
    input()

def mostrar_acelerometro():
    """Mostrar datos del acelerómetro"""
    print("🚀 Iniciando captura línea por línea con buffer inteligente...")

    output_image = "/data/graf.png"
    buffer = deque(maxlen=150)
    sample_count = 0
    current_data_block = ""
    capturing = False

    # Ejecutar see_workhorse
    print("📡 Ejecutando see_workhorse...")
    proc = subprocess.Popen(
        ["see_workhorse", "-debug=1", "-display_events=1", "-sensor=accel", "-sample_rate=50", "-duration=60"],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        universal_newlines=True,
        bufsize=1
    )

    print("🔍 Escuchando datos (multi-línea)...")

    for line in iter(proc.stdout.readline, ''):
        if line.strip() == "":
            continue

        if '"data"' in line:
            capturing = True
            current_data_block = line
            continue

        if capturing:
            current_data_block += line
            if "]" in line:
                try:
                    start = current_data_block.index("[")
                    end = current_data_block.index("]")
                    data_str = current_data_block[start + 1:end]
                    values = [float(x.strip()) for x in data_str.split(",")]

                    buffer.append(values)
                    sample_count += 1
                    print(f"📈 Muestra {sample_count}: {values}")

                    magnitude = math.sqrt(sum(v ** 2 for v in values))
                    if magnitude > 15:
                        print(f"🛑 Magnitud > 15 detectada: {magnitude:.2f}. Deteniendo...")
                        proc.terminate()
                        break
                except Exception as e:
                    print(f"⚠️ Error al parsear bloque: {e}")
                finally:
                    capturing = False
                    current_data_block = ""

        if proc.poll() is not None:
            break

    # Verificar datos
    if not buffer:
        print("⚠️ No se capturaron datos.")
        print("Presiona Enter para volver al menú...")
        input()
        return

    print(f"✅ Se capturaron {sample_count} muestras. Generando gráfica...")

    xs, ys, zs = zip(*buffer)

    plt.figure(figsize=(8, 4))
    plt.plot(xs, label='X', linewidth=1.2)
    plt.plot(ys, label='Y', linewidth=1.2)
    plt.plot(zs, label='Z', linewidth=1.2)
    plt.axhline(0, color='gray', linestyle='--', linewidth=0.5)
    plt.legend()
    plt.title("Acelerómetro - Últimos 150 valores")
    plt.xlabel("Muestra")
    plt.ylabel("g")
    plt.tight_layout()
    plt.savefig(output_image)
    plt.close()  # Cerrar la figura para liberar memoria

    print(f"📷 Imagen guardada en {output_image}")
    print("📊 Mostrando gráfica...")
    
    # Mostrar la gráfica
    imagen_proc = subprocess.Popen(['weston-image', output_image])
    
    # Esperar 5 segundos para ver la gráfica
    time.sleep(5)
    
    # Cerrar el visor de imagen
    imagen_proc.terminate()
    imagen_proc.wait()
    
    print("✅ Gráfica mostrada")
    print("Presiona Enter para volver al menú...")
    input()

def main(): 
    #Preparing system
    print("Turning screen off...")
    subprocess.run(['/etc/initscripts/init_qti_display', 'stop'], capture_output=True, text=True)
    print("Turning screen back on...")
    os.environ['XDG_RUNTIME_DIR'] = '/run/user/root'
    os.system('weston --tty=2 &')
    print("... done")
    
    # Menu principal
    while True:
        print("\n" + "="*50)
        print("           MENU PRINCIPAL")
        print("="*50)
        print("1. Prender/Apagar Flash")
        print("2. Tomar Foto y Mostrarla")
        print("3. Ver Datos del Acelerómetro")
        print("4. Salir")
        print("="*50)
        
        opcion = input("Selecciona una opción (1-4): ").strip()
        
        if opcion == "1":
            flash_control()
        elif opcion == "2":
            tomar_foto()
        elif opcion == "3":
            mostrar_acelerometro()
        elif opcion == "4":
            print("👋 Saliendo del programa...")
            break
        else:
            print("⚠️ Opción inválida. Por favor selecciona 1-4.")
    
    return 0

if __name__ == '__main__':
    main() 