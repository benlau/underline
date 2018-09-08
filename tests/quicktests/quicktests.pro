QT       += testlib qml quick

TARGET = quicktests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES +=     main.cpp \
    quicktests.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES += QUICK_TEST_SOURCE_DIR=\\\"$$PWD/qmltests\\\"
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

ROOT_DIR = $$absolute_path($$PWD/../..)
INCLUDEPATH += $$ROOT_DIR/src/cpp

include(../qpm.pri)

DISTFILES +=    \
    ../../.travis.yml \
    ../../README.md \
    ../../appveyor.yml \
    SampleData1.json \
    SampleData2.json

HEADERS += \
    quicktests.h

