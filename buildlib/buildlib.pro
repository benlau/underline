TARGET = underline
TEMPLATE = lib

ROOT_DIR = $$absolute_path($$PWD/..)

isEmpty(SHARED): SHARED = "false"

DEFAULT_INSTALL_ROOT = $$[QT_INSTALL_LIBS]

isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT = $${DEFAULT_INSTALL_ROOT}
}

include($$ROOT_DIR/src/underline.pri)

INSTALLS += target

isEmpty(INSTALL_ROOT): INSTALL_ROOT=$$[QT_INSTALL_LIBS]

# Uncomment the following lines to install header file
#headers.files += $$ROOT_DIR/src/xxx.h
target.path = $${INSTALL_ROOT}/lib
INSTALLS += headers

headers.path = $${INSTALL_ROOT}/include


equals(SHARED, "false") {
    CONFIG += staticlib
}

