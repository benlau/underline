
TARGET = template

CONFIG   += console
CONFIG   -= app_bundle
ROOT_DIR = $$absolute_path($$PWD/..)
INCLUDEPATH += $$ROOT_DIR/src/cpp

TEMPLATE = app

SOURCES += \
    template.cpp
