SUMMARY = "Visor de imágenes simple para Weston"
DESCRIPTION = "Muestra una imagen PNG en pantalla usando Weston"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"


SRC_URI = "file://imgviewer.c"
S = "${WORKDIR}"

inherit pkgconfig

DEPENDS = "wayland wayland-native cairo libpng"

do_compile() {
    ${CC} ${CFLAGS} ${LDFLAGS} imgviewer.c \
        `pkg-config --cflags --libs cairo libpng` \
        -o imgviewer
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 imgviewer ${D}${bindir}/imgviewer
}
