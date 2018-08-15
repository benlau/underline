#include <QQmlApplicationEngine>
#include <QTest>
#include <Automator>
#include <QtShell>
#include <functional>
#include <QMap>
#include <vector>
#include <QtCore>
#include <QVector>
#include <memory>
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

void C11TestCases::test_private_has()
{
    class A{

    };

    QCOMPARE((bool) _::Private::has_reserve<std::vector<int>>::value, true);
    QCOMPARE((bool) _::Private::has_reserve<std::string>::value, true);
    QCOMPARE((bool) _::Private::has_reserve<QVector<int>>::value, true);

    QCOMPARE((bool) _::Private::has_reserve<C11TestCases>::value, false);

    /* has_operator_round_backets_int */

    QCOMPARE((bool) _::Private::has_operator_round_backets_int<std::vector<int>>::value, true);
    QCOMPARE((bool) _::Private::has_operator_round_backets_int<const std::vector<int>>::value, true);

    QCOMPARE((bool) _::Private::has_operator_round_backets_int<std::string>::value, true);
    QCOMPARE((bool)
    _::Private::has_operator_round_backets_int<QVector<int>>::value, true);
    QCOMPARE((bool)
    _::Private::has_operator_round_backets_int<const QVector<int>>::value, true);
    QCOMPARE((bool) _::Private::has_operator_round_backets_int<C11TestCases>::value, false);

    QCOMPARE((bool)( _::Private::has_operator_round_backets_int<std::map<int,int>>::value), false);

    /* has_operator_round_backets_key */

    QCOMPARE((bool)( _::Private::has_operator_round_backets_key<std::map<std::string,int>>::value), true);

    QCOMPARE((bool)( _::Private::has_operator_round_backets_key<C11TestCases>::value), false);

    QCOMPARE((bool)( _::Private::has_operator_round_backets_key<std::vector<int>>::value), false);

    QCOMPARE((bool)( _::Private::has_operator_round_backets_key<QVector<int>>::value), false);

    QCOMPARE((bool)( _::Private::has_operator_round_backets_key<QVariantMap>::value), true);


    /* has_push_back */

    QCOMPARE((bool) _::Private::has_push_back<QVector<int>>::value, true);
    QCOMPARE((bool) _::Private::has_push_back< QList<QString>>::value, true);
    QCOMPARE((bool) _::Private::has_push_back<std::vector<A>>::value, true);
    QCOMPARE((bool) _::Private::has_push_back<std::string>::value, true);
    QCOMPARE((bool) _::Private::has_push_back<C11TestCases>::value, false);

    /* has_mapped_type */

    QCOMPARE((bool) _::Private::has_mapped_type<QVariantMap>::value, true);
    QCOMPARE((bool) _::Private::has_mapped_type<C11TestCases>::value, false);

    /* has_key_type */

    QCOMPARE((bool) _::Private::has_key_type<QVariantMap>::value, true);
    QCOMPARE((bool) _::Private::has_key_type<C11TestCases>::value, false);
}

void C11TestCases::test_private_is_array()
{
    QCOMPARE((bool)(_::Private::is_array<std::vector<int>>::value), true);
    QCOMPARE((bool)(_::Private::is_array<QStringList>::value), true);

    QCOMPARE((bool)(_::Private::is_array<QVariantList>::value), true);

    QCOMPARE((bool)(_::Private::is_array<std::string>::value), true);

    QCOMPARE((bool)(_::Private::is_array<std::list<std::string>>::value), false);

    QCOMPARE((bool)(_::Private::is_array<QVector<int>>::value), true);
    QCOMPARE((bool)(_::Private::is_array<QList<int>>::value), true);

    QCOMPARE((bool)(_::Private::is_array<C11TestCases>::value), false);

    QCOMPARE((bool)(_::Private::is_array<std::map<int, bool>>::value), false);

    QCOMPARE((bool)(_::Private::is_array<QMap<int, bool>>::value), false);

    QCOMPARE((bool)(_::Private::is_array<int>::value), false);
}

void C11TestCases::test_private_is_map()
{
    QCOMPARE((bool)(_::Private::is_map<std::vector<int>>::value), false);
    QCOMPARE((bool)(_::Private::is_map<std::string>::value), false);
    QCOMPARE((bool)(_::Private::is_map<std::list<std::string>>::value), false);
    QCOMPARE((bool)(_::Private::is_map<QVector<int>>::value), false);
    QCOMPARE((bool)(_::Private::is_map<QList<int>>::value), false);

    QCOMPARE((bool)(_::Private::is_map<std::map<int, std::string>>::value), true);
    QCOMPARE((bool)(_::Private::is_map<std::unordered_map<int, std::string>>::value), true);
    QCOMPARE((bool)(_::Private::is_map<QMap<int, QString>>::value), true);

    QCOMPARE((bool)(_::Private::is_map<QVariantMap>::value), true);

    QVERIFY((std::is_same<_::Private::map_mapped_type_t<QVariantMap>, QVariant>::value));

    QVERIFY((std::is_same<_::Private::map_info<C11TestCases>::mapped_type, _::Private::Undefined>::value));

    QVERIFY((std::is_same<_::Private::map_info<QVariantMap>::mapped_type, QVariant>::value));

    QVERIFY(!(std::is_same<_::Private::map_mapped_type_t<C11TestCases>, QVariant>::value));

}

void C11TestCases::test_private_is_meta_object()
{
    QCOMPARE((bool)(_::Private::is_meta_object<QObject>::value), true);
    QCOMPARE((bool)(_::Private::is_meta_object<QObject*>::value), true);
    QCOMPARE((bool)(_::Private::is_meta_object<QObject**>::value), false);

    QCOMPARE((bool)(_::Private::is_meta_object<GadgetObject>::value), true);
    QCOMPARE((bool)(_::Private::is_meta_object<GadgetObject*>::value), true);
    QCOMPARE((bool)(_::Private::is_meta_object<GadgetObject**>::value), false);

    QCOMPARE((bool)(_::Private::is_meta_object<QString>::value), false);
}

void C11TestCases::test_private_is_qobject()
{
    QCOMPARE((bool)_::Private::is_qobject<QObject>::value, true);
    QCOMPARE((bool)_::Private::is_qobject<QObject*>::value, true);

    QCOMPARE((bool)_::Private::is_qobject<C11TestCases>::value, true);
    QCOMPARE((bool)_::Private::is_qobject<C11TestCases*>::value, true);

    QCOMPARE((bool)_::Private::is_qobject<GadgetObject>::value, false);
}

void C11TestCases::test_private_is_key_value_type()
{
    QCOMPARE((bool)_::Private::is_key_value_type<QObject>::value, true);
    QCOMPARE((bool)_::Private::is_key_value_type<QObject*>::value, true);

    QCOMPARE((bool)_::Private::is_key_value_type<C11TestCases>::value, true);
    QCOMPARE((bool)_::Private::is_key_value_type<C11TestCases*>::value, true);

    QCOMPARE((bool)_::Private::is_key_value_type<GadgetObject>::value, true);
    QCOMPARE((bool)_::Private::is_key_value_type<QVariantMap>::value, true);

    QCOMPARE((bool)_::Private::is_key_value_type<int>::value, false);

}

template <typename F, typename T>
auto wrapper(F functor, T t) -> typename _::Private::ret_func<F,T>::type {
    return functor(t);
}

void C11TestCases::test_private_traits()
{
    {
        // rebind
        const std::type_info& ti1 = typeid(_::Private::array_rebinder<QList<int>, QString>::type);
        const std::type_info& ti2 = typeid(QList<QString>);

        QVERIFY(ti1 == ti2);
    }

    {

        // container_value_type

        const std::type_info& ti1 = typeid(_::Private::array_value_type<QList<int>>::type);
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
        const std::type_info& ti1 = typeid(_::Private::array_rebinder<QList<int>, QString>::type);
        const std::type_info& ti2 = typeid(QList<QString>);

        QVERIFY(ti1 == ti2);

    }

    {
        const std::type_info& ti1 = typeid(_::Private::array_rebinder<QStringList, int>::type);
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

void C11TestCases::test_private_invoke_by_read()
{
    {
        /* QMap */
        QMap<QString, int> map{{"value1", 1}};

        QCOMPARE(_::Private::invoke("value1", map), 1);
    }
}

void C11TestCases::test_private_rebind_to_map()
{

    QCOMPARE((std::is_same<std::map<std::string,int>,
                           _::Private::array_to_map_rebinder<std::list<int>,std::string, int>::type
                           >::value), true);

    QCOMPARE((std::is_same<QMap<QString,int>,
                           _::Private::array_to_map_rebinder<QList<int>,QString, int>::type
                           >::value), true);



}

class DummyClassWithStaticMetaObject {
public:
    static int staticMetaObject;
};

void C11TestCases::test_private_has_static_meta_object()
{
    class A {

    };

    QCOMPARE((bool) (_::Private::has_static_meta_object<A>::value), false);
    QCOMPARE((bool) (_::Private::has_static_meta_object<DataObject*>::value), true);
    QCOMPARE((bool) (_::Private::has_static_meta_object<GadgetObject>::value), true);
    QCOMPARE((bool) (_::Private::has_static_meta_object<GadgetObject*>::value), true);


    QCOMPARE((bool) (_::Private::has_static_meta_object<DummyClassWithStaticMetaObject>::value), false);

}

void C11TestCases::test_private_read()
{
    // Map
    QCOMPARE((_::Private::read(std::map<std::string, int>{{"value1", 1}}, std::string("value1"))), 1);

    QCOMPARE((_::Private::read(std::map<std::string, int>{{"value1", 1}}, std::string("value0"))), 0);

    QCOMPARE((_::Private::read(QMap<QString, int>{{"value1", 1}}, QString("value1"))), 1);

    QCOMPARE((_::Private::read(QVariantMap{{"value1", 1}}, QString("value1"))), QVariant(1));

    QVERIFY( (std::is_same<_::Private::ret_read<QVariantMap, QString>::type, QVariant>::value));

    // Array
    QCOMPARE((_::Private::read(std::vector<int>{0,1,2}, 1)), 1);
    QCOMPARE((_::Private::read(QVector<int>{0,1,2}, 1)), 1);

    // Gadget Object
    GadgetObject gadget;
    gadget.value = 10;
    QCOMPARE((_::Private::meta_object_value(&gadget, "value")), QVariant(10));
    QCOMPARE((_::Private::meta_object_value(gadget, "value")), QVariant(10));
    QCOMPARE((_::Private::meta_object_value(gadget, "value1")), QVariant());

    QVERIFY((_::Private::is_meta_object_key_matched<decltype(gadget), decltype("value")>::value));
    QVERIFY(!(_::Private::is_map_key_matched<decltype(gadget), decltype("value")>::value));
    QVERIFY(!(_::Private::is_array_index_matched<decltype(gadget), decltype("value")>::value));

    QCOMPARE((_::Private::read(&gadget, "value")), QVariant(10));
    QCOMPARE((_::Private::read(gadget, "value")), QVariant(10));
    QCOMPARE((_::Private::read(gadget, "value1")), QVariant());

    QObject* object = new QObject(this);
    object->setObjectName("objectName");
    QCOMPARE((_::Private::meta_object_value(object, "objectName")), QVariant("objectName"));
    QCOMPARE((_::Private::read(object, "objectName")), QVariant("objectName"));

}

void C11TestCases::test_private_write()
{
    /* Map */

    {
        std::map<std::string, int> map;
        _::Private::write(map, "value1", 1);
        QCOMPARE(map["value1"], 1);
    }

    {
        QMap<QString, int> map;
        _::Private::write(map, "value1", 1);
        QCOMPARE(map["value1"], 1);
    }

    /* Gadget */

    {
        GadgetObject object;
        object.value = 0;

        _::Private::write(object, "value", 1);
        QCOMPARE(object.value, 1);

        _::Private::write(&object, "value", 2);
        QCOMPARE(object.value, 2);

    }

    {
        std::unique_ptr<DataObject> object(new DataObject(this));

        object->setValue1(0);
        _::Private::write(object.get(), "value1", 1);
        QCOMPARE(object->value1(), 1);
    }
}

void C11TestCases::test_cast_to_pointer()
{
    class A {
    public:
        int value;
    };

    A a;
    a.value = 10;
    QCOMPARE(_::Private::cast_to_pointer(a)->value, 10);
    QCOMPARE(_::Private::cast_to_pointer(&a)->value, 10);
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

void C11TestCases::test_isMap()
{
    QCOMPARE(_::isMap(std::vector<int>{}),    false);
    QCOMPARE(_::isMap(QVector<int>{ }),       false);
    QCOMPARE(_::isMap(QList<int>{ }),         false);
    QCOMPARE(_::isMap(QVariantList{ }),       false);
    QCOMPARE(_::isMap(QString{ }),            false);
    QCOMPARE(_::isMap(10),                    false);

    QCOMPARE(_::isMap(std::map<bool,int>{}),  true);
    QCOMPARE(_::isMap(QMap<int,int>{}),       true);
    QCOMPARE(_::isMap(new C11TestCases(this)),          true);
}



