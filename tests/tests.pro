QT       += testlib qml

TARGET = tests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES +=     main.cpp \    
    testcases.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES += QUICK_TEST_SOURCE_DIR=\\\"$$PWD/qmltests\\\"
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

ROOT_DIR = $$absolute_path($$PWD/..)
INCLUDEPATH += $$ROOT_DIR/src/cpp

include(qpm.pri)

DISTFILES +=     qpm.json     \
    ../.travis.yml \
    ../README.md \
    ../appveyor.yml \
    qpm.pri

!win32 {
    QMAKE_CXXFLAGS += -Werror
}

HEADERS += \
    testcases.h

win32 {
    CONFIG(debug, debug|release) {
        QMAKE_LIBDIR += $${OUT_PWD}/../buildlib/debug
        PRE_TARGETDEPS += $${OUT_PWD}/../buildlib/debug/underline.lib
    } else {
        QMAKE_LIBDIR += $${OUT_PWD}/../buildlib/release
        PRE_TARGETDEPS += $${OUT_PWD}/../buildlib/release/underline.lib
    }

} else {
    LIBS += -L$${OUT_PWD}/../buildlib
    PRE_TARGETDEPS += $$absolute_path($${OUT_PWD}/../buildlib/libunderline.a)
}

LIBS += -lunderline
