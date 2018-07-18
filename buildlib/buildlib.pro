TEMPLATE = aux
QT       += core

ROOT_DIR = $$absolute_path($$PWD/..)
include($$ROOT_DIR/src/underline.pri)

DEFAULT_INSTALL_ROOT = $$[QT_INSTALL_PREFIX]
isEmpty(INSTALL_ROOT): INSTALL_ROOT=$${DEFAULT_INSTALL_ROOT}

message($$ROOT_DIR)
headers.files += $$ROOT_DIR/src/cpp/underline.h
headers.path = $${INSTALL_ROOT}/include
INSTALLS += headers

