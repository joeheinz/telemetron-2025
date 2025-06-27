SUMMARY = "Hello World Python script for Quectel"
LICENSE = "MIT"
#LIC_FILES_CHKSUM = "file://LICENSE;md5=0835c6ed6b27b5b7fc4d6a09d1b89b17"
#LIC_FILES_CHKSUM = "file://LICENSE;md5=${@bb.utils.md5_file('${THISDIR}/files/LICENSE')}"
#LIC_FILES_CHKSUM = "file://LICENSE;md5=0835c6ed6b27b5b7fc4d6a09d1b89b17"
#LIC_FILES_CHKSUM = "file://files/LICENSE;md5=3a034361bb5a6d7bc3b39cec7426b13a"
#LIC_FILES_CHKSUM = "file://LICENSE;md5=3a034361bb5a6d7bc3b39cec7426b13a"
LIC_FILES_CHKSUM = "file://LICENSE;md5=41a9dfb529ec3642f6f50bf06e6ba843"


#SRC_URI = "file://hello.py"

SRC_URI = "file://hello.py file://LICENSE"


S = "${WORKDIR}"

RDEPENDS:${PN} += "python3-core"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/hello.py ${D}${bindir}/hello-python
}
