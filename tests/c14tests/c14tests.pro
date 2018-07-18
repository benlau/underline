QT       += testlib qml

TARGET = c14tests
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++14

TEMPLATE = app

SOURCES +=     main.cpp \     
    c14testcases.cpp \
    dataobject.cpp \
    dataobject2.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES += QUICK_TEST_SOURCE_DIR=\\\"$$PWD/qmltests\\\"
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

ROOT_DIR = $$absolute_path($$PWD/../..)
INCLUDEPATH += $$ROOT_DIR/src/cpp

include(../qpm.pri)

DISTFILES +=    \
    ../../.travis.yml \
    ../../README.md \
    ../../appveyor.yml

!win32 {
    QMAKE_CXXFLAGS += -Werror
}

HEADERS += \
    c14testcases.h \
    dataobject.h \
    dataobject2.h

