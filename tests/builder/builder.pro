QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle
CONFIG += c++14

TEMPLATE = app

ROOT_DIR = $$absolute_path($$PWD/../..)
INCLUDEPATH += $$ROOT_DIR/src/cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../qpm.pri)

SOURCES +=  tst_builder.cpp
