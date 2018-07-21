#include <QQmlApplicationEngine>
#include <QTest>
#include <Automator>
#include <QtShell>
#include <functional>
#include <QMap>
#include "c11testcases.h"
#include "underline.h"

static bool isOdd(int value) {
    return value % 2 == 1;
}

C11TestCases::C11TestCases(QObject *parent) : QObject(parent)
{
    auto ref = [=]() {
        QTest::qExec(this, 0, 0); // Autotest detect available test cases of a QObject by looking for "QTest::qExec" in source code
    };
    Q_UNUSED(ref);
}

template <typename F, typename T>
auto wrapper(F functor, T t) -> typename _::Private::ret_func<F,T>::type {
    return functor(t);
}

void C11TestCases::test_private_traits()
{
    {
        // rebind
        const std::type_info& ti1 = typeid(_::Private::rebind<QList<int>, QString>::type);
        const std::type_info& ti2 = typeid(QList<QString>);

        QVERIFY(ti1 == ti2);
    }

    {

        // container_value_type

        const std::type_info& ti1 = typeid(_::Private::container_value_type<QList<int>>::type);
        const std::type_info& ti2 = typeid(int);

        QVERIFY(ti1 == ti2);

    }

    {
        const std::type_info& ti1 = typeid(std::remove_reference<QMap<QString,int>>::type::mapped_type);
        const std::type_info& ti2 = typeid(int);
        QVERIFY(ti1 == ti2);
    }

    {
        // rebind
        const std::type_info& ti1 = typeid(_::Private::rebind<QList<int>, QString>::type);
        const std::type_info& ti2 = typeid(QList<QString>);

        QVERIFY(ti1 == ti2);

    }

    {
        const std::type_info& ti1 = typeid(_::Private::rebind<QStringList, int>::type);
        const std::type_info& ti2 = typeid(QList<int>);

        QVERIFY(ti1 == ti2);

    }
}

void C11TestCases::test_private_invoke()
{
    auto myFunc0 = []() {
        return -1;
    };

    auto myFunc1 = [](int i) {
        return i;
    };

    auto myFunc2 = [](int i, int j) {
        Q_UNUSED(j);
        return i;
    };

    auto myFunc2_intToString = [](QString i, int j) {
        Q_UNUSED(j);
        return i.toInt();
    };

    QCOMPARE((int) (_::Private::is_args_compatible<decltype(myFunc0)>::value), 1);
    QCOMPARE((int) (_::Private::is_args_compatible<decltype(myFunc0), int>::value), 0);

    QCOMPARE((int) (_::Private::is_args_compatible<decltype(myFunc1),int>::value), 1);
    QCOMPARE((int) (_::Private::is_args_compatible<decltype(myFunc1),int>::value), 1);
    QCOMPARE((int) (_::Private::is_args_compatible<decltype(myFunc1),QString, int>::value), 0);
    QCOMPARE((int) (_::Private::is_args_compatible<decltype(myFunc1),QString, int>::value), 0);

    QCOMPARE((std::is_same<decltype(_::Private::decl_func0<decltype(myFunc0)>()), int>::value), true);

    QCOMPARE((std::is_same<decltype(_::Private::decl_func0<decltype(myFunc1),QString>()), void>::value), true);

    QCOMPARE((std::is_same<_::Private::ret_func<decltype(myFunc0)>::type,int>::value), true);

    QCOMPARE((std::is_same<_::Private::ret_func<decltype(myFunc1),int>::type,int>::value), true);
    QCOMPARE((std::is_same<_::Private::ret_func<decltype(myFunc2),int, int>::type,int>::value), true);
    QCOMPARE((std::is_same<_::Private::ret_func<decltype(myFunc2),QString, int>::type,void>::value), true);
    QCOMPARE((std::is_same<_::Private::ret_func<decltype(myFunc2_intToString),QString, int>::type , int>::value), true);

    QCOMPARE(_::Private::invoke(myFunc0) , -1);
    QCOMPARE(_::Private::invoke(myFunc0, 1) , -1);
    QCOMPARE(_::Private::invoke(myFunc0, 2,3) , -1);

    QCOMPARE(_::Private::invoke(myFunc1, 1) , 1);
    QCOMPARE(_::Private::invoke(myFunc1, 2,3) , 2);

    QCOMPARE(_::Private::invoke(myFunc2, 2,3) , 2);

    QCOMPARE((std::is_same<decltype(_::Private::decl_invoke0<decltype(myFunc0)>()) , int>::value), true);

    QCOMPARE((std::is_same<decltype(_::Private::decl_invoke0<decltype(myFunc2), int, int>()) , int>::value), true);

    QCOMPARE((std::is_same<_::Private::ret_invoke<decltype(myFunc0),QString, int>::type , int>::value), true);

    QCOMPARE((bool) (_::Private::is_invokable3<decltype(myFunc2), int, int, int>::value), true);
    QCOMPARE((bool) (_::Private::is_invokable3<decltype(myFunc2), int, QString, int>::value), false);
}

void C11TestCases::test_some()
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
        /// test non-int function
        auto func = [](QString item) {
            return item.size() % 2 == 1;
        };

        QCOMPARE(_::some(QStringList() << "0" << "2" << "4", func), true);
        QCOMPARE(_::some(QStringList() << "01" << "21" << "42", func), false);

    }

}

void C11TestCases::test_map()
{
    {
        auto func = [](QString item) {
            return item.toInt();
        };

        QCOMPARE(_::map(QList<QString>() << "1" << "2" << "3", func), QList<int>() << 1 << 2 << 3);
        QCOMPARE(_::map(QVector<QString>() << "1" << "2" << "3", func), QVector<int>() << 1 << 2 << 3);
    }
}


