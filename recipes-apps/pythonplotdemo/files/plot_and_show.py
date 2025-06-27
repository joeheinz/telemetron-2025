#!/usr/bin/env python3

import matplotlib
matplotlib.use('Agg')  # Sin GUI
import matplotlib.pyplot as plt
import os
import subprocess

# Crear gráfico simple
plt.plot([0, 1, 2, 3], [0, 1, 4, 9])
plt.title("Ejemplo desde matplotlib en el EVB")
plt.xlabel("X")
plt.ylabel("Y")

# Guardar la imagen
out_path = "/data/plot.png"
plt.savefig(out_path)

# Mostrar con weston-image si existe
if os.path.exists("/usr/bin/weston-image"):
    subprocess.run(["weston-image", out_path])
else:
    print("Imagen generada:", out_path)
