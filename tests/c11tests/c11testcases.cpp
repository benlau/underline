#include <QQmlApplicationEngine>
#include <QTest>
#include <Automator>
#include <QtShell>
#include <functional>
#include <QMap>
#include <vector>
#include <QtCore>
#include <QVector>
#include "c11testcases.h"
#include "underline.h"
#include "dataobject.h"
#include "gadgetobject.h"

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

void C11TestCases::test_is_collection()
{
    QCOMPARE((bool)(_::Private::is_collection<std::vector<int>>::value), true);
    QCOMPARE((bool)(_::Private::is_collection<std::string>::value), false);

    QCOMPARE((bool)(_::Private::is_collection<std::list<std::string>>::value), true);

    QCOMPARE((bool)(_::Private::is_collection<QVector<int>>::value), true);
    QCOMPARE((bool)(_::Private::is_collection<QList<int>>::value), true);

}

void C11TestCases::test_is_map()
{
    QCOMPARE((bool)(_::Private::is_map<std::vector<int>>::value), false);
    QCOMPARE((bool)(_::Private::is_map<std::string>::value), false);
    QCOMPARE((bool)(_::Private::is_map<std::list<std::string>>::value), false);
    QCOMPARE((bool)(_::Private::is_map<QVector<int>>::value), false);
    QCOMPARE((bool)(_::Private::is_map<QList<int>>::value), false);

    QCOMPARE((bool)(_::Private::is_map<std::map<int, std::string>>::value), true);
    QCOMPARE((bool)(_::Private::is_map<std::unordered_map<int, std::string>>::value), true);
    QCOMPARE((bool)(_::Private::is_map<QMap<int, QString>>::value), true);
}

void C11TestCases::test_is_meta_object()
{
    QCOMPARE((bool)(_::Private::is_meta_object<QObject*>::value),true);
    QCOMPARE((bool)(_::Private::is_meta_object<QString>::value),false);

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

        const std::type_info& ti1 = typeid(_::Private::collection_value_type<QList<int>>::type);
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

    QCOMPARE((std::is_same<_::Private::ret_invoke<QString,QString, int>::type , _::Private::Undefined>::value), true);

    QCOMPARE((std::is_same<decltype(_::Private::decl_invoke0<decltype(myFunc0)>()) , int>::value), true);

    QCOMPARE((std::is_same<decltype(_::Private::decl_invoke0<decltype(myFunc2), int, int>()) , int>::value), true);

    QCOMPARE((std::is_same<_::Private::ret_invoke<decltype(myFunc0),QString, int>::type , int>::value), true);

    QCOMPARE((bool) (_::Private::is_invokable3<decltype(myFunc2), int, int, int>::value), true);
    QCOMPARE((bool) (_::Private::is_invokable3<decltype(myFunc2), int, QString, int>::value), false);
}

void C11TestCases::test_private_rebind_to_map()
{
    QCOMPARE((std::is_same<QMap<QString,int>,
                           _::Private::rebind_to_value_map<QList<QString>, int>::type
                           >::value), true);

    QCOMPARE((std::is_same<QMap<std::string,int>,
                           _::Private::rebind_to_value_map<QVector<int>, int>::type
                           >::value), false);

    QCOMPARE((std::is_same<QMap<int,int>,
                           _::Private::rebind_to_value_map<QList<int>, int>::type
                           >::value), true);


    QCOMPARE((std::is_same<std::map<std::string,int>,
                           _::Private::rebind_to_key_value_map<std::list<int>,std::string, int>::type
                           >::value), true);

    QCOMPARE((std::is_same<QMap<QString,int>,
                           _::Private::rebind_to_key_value_map<QList<int>,QString, int>::type
                           >::value), true);



}

void C11TestCases::test_has_static_meta_object()
{
    class A {

    };

    QCOMPARE((bool) (_::Private::has_static_meta_object<A>::value), false);
    QCOMPARE((bool) (_::Private::has_static_meta_object<GadgetObject>::value), true);

}

void C11TestCases::test_private_read()
{
    // Map
    QCOMPARE((_::Private::read(std::map<std::string, int>{{"value1", 1}}, std::string("value1"))), 1);

    QCOMPARE((_::Private::read(std::map<std::string, int>{{"value1", 1}}, std::string("value0"))), 0);

    QCOMPARE((_::Private::read(QMap<QString, int>{{"value1", 1}}, QString("value1"))), 1);

    // Collection
    QCOMPARE((_::Private::read(std::vector<int>{0,1,2}, 1)), 1);
    QCOMPARE((_::Private::read(QVector<int>{0,1,2}, 1)), 1);    
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

void C11TestCases::test_range()
{
    QCOMPARE(_::range<std::vector<int>>(4), (std::vector<int>{0,1,2,3}));

    QCOMPARE(_::range<QList<int>>(-4), (QList<int>{0,-1,-2,-3}));

    QCOMPARE(_::range<QVector<int>>(1,5), (QVector<int>{1,2,3,4}));

    QCOMPARE(_::range<QList<int>>(0, 20, 5), (QList<int>{0, 5, 10, 15}));
    QCOMPARE(_::range<QList<int>>(0, 20, 5.0), (QList<int>{0, 5, 10, 15}));

    QCOMPARE(_::range<std::vector<int>>(1,4,0), (std::vector<int>{1,1,1}));

    QCOMPARE(_::range<QList<int>>(0), (QList<int>{}));
}

void C11TestCases::test_range_q()
{
    QCOMPARE(_::range_q(4), (QList<int>{0,1,2,3}));

    QCOMPARE(_::range_q(-4), (QList<int>{0,-1,-2,-3}));
    QCOMPARE(_::range_q(1,5), (QList<int>{1,2,3,4}));
    QCOMPARE(_::range_q(0, 20, 5), (QList<int>{0, 5, 10, 15}));
    QCOMPARE(_::range_q(0, 20, 5.0), (QList<int>{0, 5, 10, 15}));
    QCOMPARE(_::range_q(1,4,0), (QList<int>{1,1,1}));
    QCOMPARE(_::range_q(0), (QList<int>{}));
}



