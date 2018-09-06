#include <underline.h>
#include <gtest/gtest.h>
#include <math.h>
#include <list>

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

TEST(UnderlineTests, test_has_static_method) {

    class A {

    };

    ASSERT_EQ(static_cast<bool> (_::Private::has_static_meta_object<A>::value), false);

}

TEST(UnderlineTests, is_qobject) {

    class A {

    };

    ASSERT_EQ(static_cast<bool> (_::Private::is_qobject<A>::value), false);
}

TEST(UnderlineTests, isMap) {
    ASSERT_EQ(_::isMap(std::map<int,int>{}),   true);

    ASSERT_EQ(_::isMap(10),                    false);
    ASSERT_EQ(_::isMap(std::vector<int>{}),    false);
}

TEST(UnderlineTests, isKeyValueType) {
    ASSERT_EQ(_::isKeyValueType(std::map<int,int>{}),   true);

    ASSERT_EQ(_::isKeyValueType(10),                    false);
    ASSERT_EQ(_::isKeyValueType(std::vector<int>{}),    false);
}


int main(int argc, char** argv) {

#ifdef QT_CORE_LIB
    static_assert(True, "Qt Core is defined");
#endif
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
