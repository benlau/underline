#include <underline.h>
#include <gtest/gtest.h>
#include <math.h>

TEST(UnderlineTests, test_some) {
    {
        auto func = [](int item) {
            return item % 2 == 1;
        };

        ASSERT_EQ(_::some(std::vector<int>{0, 2, 4}, func), false);
        ASSERT_EQ(_::some(std::vector<int>{0, 3, 4}, func), true);
    }
}

TEST(UnderlineTests, test_clamp) {

    ASSERT_EQ(_::clamp(5,4,7), 5);
    ASSERT_EQ(_::clamp(-7,-4,7), -4);
    ASSERT_EQ(_::clamp(1,-20,-10), -10);
}

int main(int argc, char** argv) {

#ifdef QT_CORE_LIB
    static_assert(True, "Qt Core is defined");
#endif
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
