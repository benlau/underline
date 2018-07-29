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

TEST(UnderlineTests, test_private_rebind_to_map) {

    ASSERT_EQ((std::is_same<std::map<std::string,int>,
                           _::Private::rebind_to_value_map<std::vector<std::string>, int>::type
                           >::value), true);

    ASSERT_EQ((std::is_same<std::map<std::string,int>,
                           _::Private::rebind_to_value_map<std::vector<int>, int>::type
                           >::value), false);

    ASSERT_EQ((std::is_same<std::map<int,int>,
                           _::Private::rebind_to_value_map<std::list<int>, int>::type
                           >::value), true);

}

int main(int argc, char** argv) {

#ifdef QT_CORE_LIB
    static_assert(True, "Qt Core is defined");
#endif
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
