#include <stdio.h>
#include <stdlib.h>
#include <wayland-client.h>
#include <cairo/cairo.h>


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <imagen.png>\n", argv[0]);
        return 1;
    }

    cairo_surface_t *imagen = cairo_image_surface_create_from_png(argv[1]);
    int ancho = cairo_image_surface_get_width(imagen);
    int alto  = cairo_image_surface_get_height(imagen);

    cairo_surface_t *ventana = cairo_image_surface_create(CAIRO_FORMAT_RGB24, ancho, alto);
    cairo_t *cr = cairo_create(ventana);

    cairo_set_source_surface(cr, imagen, 0, 0);
    cairo_paint(cr);
    cairo_destroy(cr);

    cairo_surface_write_to_png(ventana, "/tmp/output.png");
    system("weston-image /tmp/output.png");

    cairo_surface_destroy(imagen);
    cairo_surface_destroy(ventana);
    return 0;
}
