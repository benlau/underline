#include <gtest/gtest.h>
#include <math.h>
#include <list>
#include <QCoreApplication>

TEST(c17tests, test_version) {
    ASSERT_EQ(__cplusplus, 201703L);
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    Q_UNUSED(app);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
