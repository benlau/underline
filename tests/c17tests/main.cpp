#include <gtest/gtest.h>
#include <math.h>
#include <list>
#include <QCoreApplication>
#include <QtCore>

TEST(c17tests, test_version) {
    qDebug() << "__cplusplus" << __cplusplus;
    qDebug() << "__cpp_hex_float" << __cpp_hex_float;
    ASSERT_LE(201406, __cplusplus);
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    Q_UNUSED(app);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
