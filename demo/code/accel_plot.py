import subprocess
import math
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from collections import deque

print("🚀 Iniciando captura línea por línea con buffer inteligente...")

output_image = "/data/g2.png"
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
                    print(f"🛑 Magnitud > 10 detectada: {magnitude:.2f}. Deteniendo...")
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
    exit(1)

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

print(f"📷 Imagen guardada en {output_image}")
print("🏁 Script terminado.")

