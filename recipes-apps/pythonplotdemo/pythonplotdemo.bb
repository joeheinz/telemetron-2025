SUMMARY = "Demo de Python con matplotlib y weston-image"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://plot_and_show.py"

S = "${WORKDIR}"

RDEPENDS:${PN} = "python3 python3-matplotlib"


do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/plot_and_show.py ${D}${bindir}/pythonplotdemo
}
