#include <QQmlApplicationEngine>
#include <QTest>
#include <Automator>
#include <QtShell>
#include <functional>
#include "testcases.h"
#include "underline.h"

static bool isOdd(int value) {
    return value % 2 == 1;
}

TestCases::TestCases(QObject *parent) : QObject(parent)
{
    auto ref = [=]() {
        QTest::qExec(this, 0, 0); // Autotest detect available test cases of a QObject by looking for "QTest::qExec" in source code
    };
    Q_UNUSED(ref);
}


template <typename F, typename T>
auto wrapper(F functor, T t) -> typename _::Private::ret_arg1_traits<F,T>::type {
    return functor(t);
}

void TestCases::test_private_traits()
{
    {
        auto func = [](auto i) {
            Q_UNUSED(i);
        };

        QCOMPARE((std::is_convertible<std::function<void(int)>, decltype(func)>::value), false);

        QCOMPARE((std::is_convertible<decltype(func), std::function<void(int)>>::value), true);
    }

    {
        // rebind
        const std::type_info& ti1 = typeid(_::Private::rebind<QList<int>, QString>::type);
        const std::type_info& ti2 = typeid(QList<QString>);

        QVERIFY(ti1 == ti2);
    }

    {
        // ret_arg1_traits

        auto func = [](auto i) {
            return i;
        };

        const std::type_info& ti1 = typeid(_::Private::ret_arg1_traits<decltype(func), int>::type);
        const std::type_info& ti2 = typeid(int);

        QVERIFY(ti1 == ti2);

        QCOMPARE(wrapper(func, 3), 3);
    }

    {

        // container_value_type

        const std::type_info& ti1 = typeid(_::Private::container_value_type<QList<int>>::type);
        const std::type_info& ti2 = typeid(int);

        QVERIFY(ti1 == ti2);

    }

    {
        // combine container_value_type && ret_arg1_traits
        auto func = [](auto i) {
            return i;
        };

        const std::type_info& ti1 = typeid(_::Private::ret_arg1_traits<decltype(func), _::Private::container_value_type<QList<QString>>::type>::type);

        const std::type_info& ti2 = typeid(QString);

        QVERIFY(ti1 == ti2);
    }
}

void TestCases::test_private_invoke()
{
    auto func0 = []() {
        return -1;
    };

    auto func1 = [](auto i) {
        return i;
    };

    Q_UNUSED(func1);


    QCOMPARE((int) (_::Private::is_args_compatible<decltype(func0)>::value), 1);
    QCOMPARE((int) (_::Private::is_args_compatible<decltype(func0), int>::value), 0);

    QCOMPARE((int) (_::Private::is_args_compatible<decltype(func1),int>::value), 1);
    QCOMPARE((int) (_::Private::is_args_compatible<decltype(func1),QString>::value), 1);
    QCOMPARE((int) (_::Private::is_args_compatible<decltype(func1),QString, int>::value), 0);


    QCOMPARE(_::Private::invoke(func0) , -1);
    QCOMPARE(_::Private::invoke(func0, 1) , -1);
    QCOMPARE(_::Private::invoke(func1, 1) , 1);
}

void TestCases::test_some()
{
    {
        /// Standard lambda method
        auto func = [](int item, int index) {
            Q_UNUSED(index);
            return item % 2 == 1;
        };

        QCOMPARE(_::some(QList<int>() << 0 << 2 << 4, func), false);
        QCOMPARE(_::some(QList<int>() << 0 << 3 << 4, func), true);
    }

    {
        /// Standard lambda method without the index parameter
        auto func = [](int item) {
            Q_UNUSED(index);
            return item % 2 == 1;
        };

        QCOMPARE(_::some(QList<int>() << 0 << 2 << 4, func), false);
        QCOMPARE(_::some(QList<int>() << 0 << 3 << 4, func), true);
    }


    {
        /// Standard function
        QCOMPARE(_::some(QList<int>() << 0 << 2 << 4, isOdd), false);
        QCOMPARE(_::some(QList<int>() << 0 << 3 << 4, isOdd), true);
    }


    {
        auto func = [](auto item, auto index) {
            Q_UNUSED(index);
            return item % 2 == 1;
        };

        QCOMPARE(_::some(QList<int>() << 0 << 2 << 4, func), false);
        QCOMPARE(_::some(QList<int>() << 0 << 3 << 4, func), true);
    }

    {
        auto func = [](auto item) {
            return item % 2 == 1;
        };

        QCOMPARE(_::some(QList<int>() << 0 << 2 << 4, func), false);
        QCOMPARE(_::some(QList<int>() << 0 << 3 << 4, func), true);
    }

    {
        /// test non-int function
        auto func = [](QString item) {
            return item.size() % 2 == 1;
        };

        QCOMPARE(_::some(QStringList() << "0" << "2" << "4", func), true);
        QCOMPARE(_::some(QStringList() << "01" << "21" << "42", func), false);

    }

}

void TestCases::test_map()
{
    {
        auto func = [](QString item) {
            return item.toInt();
        };

        QCOMPARE(_::map(QList<QString>() << "1" << "2" << "3", func), QList<int>() << 1 << 2 << 3);
    }

    {
        // generic lambda
        auto func = [](auto item) {
            return item.toInt();
        };

        QCOMPARE(_::map(QStringList() << "1" << "2" << "3", func), QList<int>() << 1 << 2 << 3);
    }
}

