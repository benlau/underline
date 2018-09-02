#include <gtest/gtest.h>
#include <math.h>
#include <list>
#include <QCoreApplication>
#include <QtCore>

#ifndef __cpp_hex_float
#define __cpp_hex_float "Undefined"
#endif

#ifndef _MSC_VER
#define _MSC_VER "Undefined"
#endif

#ifndef _MSVC_LANG
#define _MSVC_LANG "Undefined"
#endif

TEST(c17tests, test_version) {
    qDebug() << "__cplusplus" << __cplusplus;
    qDebug() << "__cpp_hex_float" << __cpp_hex_float;

    qDebug() << "_MSC_VER" << _MSC_VER;
    qDebug() << "_MSVC_LANG" << _MSVC_LANG;

//    ASSERT_LE(201406, __cplusplus);
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    Q_UNUSED(app);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
