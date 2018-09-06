#include <QTest>
#include <functional>
#include <QMap>
#include <vector>
#include <QtCore>
#include <QVector>
#include <memory>
#include "complexqobject.h"
#include "c11testcases.h"
#include "dataobject.h"
#include "gadgetobject.h"
#define _underline_debug(x) { qDebug() << x;}
#include "underline.h"

static bool isOdd(int value) {
    return value % 2 == 1;
}

static DataObject* createMockObject(QObject* parent) {
    DataObject* ret = new DataObject(parent);
    ret->setProperty("value1", 1);
    ret->setProperty("value2", 2.0);
    ret->setProperty("value3", "3");

    return ret;
}

C11TestCases::C11TestCases(QObject *parent) : QObject(parent)
{
    auto ref = [=]() {
        QTest::qExec(this, 0, nullptr); // Autotest detect available test cases of a QObject by looking for "QTest::qExec" in source code
    };
    Q_UNUSED(ref);
}

void C11TestCases::initTestCase()
{
}

void C11TestCases::test_private_has()
{
    class A{

    };

    QCOMPARE(static_cast<bool>(_::Private::has_reserve<std::vector<int>>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::has_reserve<std::string>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::has_reserve<QVector<int>>::value), true);

    QCOMPARE(static_cast<bool>(_::Private::has_reserve<C11TestCases>::value), false);

    /* has_operator_round_backets_int */

    QCOMPARE(static_cast<bool>(_::Private::has_operator_round_backets_int<std::vector<int>>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::has_operator_round_backets_int<const std::vector<int>>::value), true);

    QCOMPARE(static_cast<bool>(_::Private::has_operator_round_backets_int<std::string>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::has_operator_round_backets_int<QVector<int>>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::has_operator_round_backets_int<const QVector<int>>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::has_operator_round_backets_int<C11TestCases>::value), false);

    QCOMPARE(static_cast<bool>( _::Private::has_operator_round_backets_int<std::map<int,int>>::value), false);

    /* has_operator_round_backets_key */

    QCOMPARE(static_cast<bool>( _::Private::has_operator_round_backets_key<std::map<std::string,int>>::value), true);

    QCOMPARE(static_cast<bool>( _::Private::has_operator_round_backets_key<C11TestCases>::value), false);

    QCOMPARE(static_cast<bool>( _::Private::has_operator_round_backets_key<std::vector<int>>::value), false);

    QCOMPARE(static_cast<bool>( _::Private::has_operator_round_backets_key<QVector<int>>::value), false);

    QCOMPARE(static_cast<bool>( _::Private::has_operator_round_backets_key<QVariantMap>::value), true);


    /* has_push_back */

    QCOMPARE(static_cast<bool>(_::Private::has_push_back<QVector<int>>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::has_push_back< QList<QString>>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::has_push_back<std::vector<A>>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::has_push_back<std::string>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::has_push_back<C11TestCases>::value), false);

    /* has_mapped_type */

    QCOMPARE(static_cast<bool>(_::Private::has_mapped_type<QVariantMap>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::has_mapped_type<C11TestCases>::value), false);

    /* has_key_type */

    QCOMPARE(static_cast<bool>(_::Private::has_key_type<QVariantMap>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::has_key_type<C11TestCases>::value), false);
}

void C11TestCases::test_private_is_collection()
{
    QCOMPARE(static_cast<bool>(_::Private::is_collection<std::vector<int>>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::is_collection<QStringList>::value), true);

    QCOMPARE(static_cast<bool>(_::Private::is_collection<QVariantList>::value), true);

    QCOMPARE(static_cast<bool>(_::Private::is_collection<std::string>::value), true);

    QCOMPARE(static_cast<bool>(_::Private::is_collection<std::list<std::string>>::value), false);

    QCOMPARE(static_cast<bool>(_::Private::is_collection<QVector<int>>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::is_collection<QList<int>>::value), true);

    QCOMPARE(static_cast<bool>(_::Private::is_collection<C11TestCases>::value), false);

    QCOMPARE(static_cast<bool>(_::Private::is_collection<std::map<int, bool>>::value), false);

    QCOMPARE(static_cast<bool>(_::Private::is_collection<QMap<int, bool>>::value), false);

    QCOMPARE(static_cast<bool>(_::Private::is_collection<int>::value), false);
}

void C11TestCases::test_private_is_map()
{
    QCOMPARE(static_cast<bool>(_::Private::is_map<std::vector<int>>::value), false);
    QCOMPARE(static_cast<bool>(_::Private::is_map<std::string>::value), false);
    QCOMPARE(static_cast<bool>(_::Private::is_map<std::list<std::string>>::value), false);
    QCOMPARE(static_cast<bool>(_::Private::is_map<QVector<int>>::value), false);
    QCOMPARE(static_cast<bool>(_::Private::is_map<QList<int>>::value), false);

    QCOMPARE(static_cast<bool>(_::Private::is_map<std::map<int, std::string>>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::is_map<std::unordered_map<int, std::string>>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::is_map<QMap<int, QString>>::value), true);

    QCOMPARE(static_cast<bool>(_::Private::is_map<QVariantMap>::value), true);

    QVERIFY((std::is_same<_::Private::map_mapped_type_t<QVariantMap>, QVariant>::value));

    QVERIFY((std::is_same<_::Private::map_mapped_type_t<C11TestCases>, QVariant>::value));

}

void C11TestCases::test_private_is_meta_object()
{
    QCOMPARE(static_cast<bool>(_::Private::is_meta_object<QObject>::value),        true);
    QCOMPARE(static_cast<bool>(_::Private::is_meta_object<QObject*>::value),       true);
    QCOMPARE(static_cast<bool>(_::Private::is_meta_object<QObject**>::value),      false);

    QCOMPARE(static_cast<bool>(_::Private::is_meta_object<GadgetObject>::value),    true);
    QCOMPARE(static_cast<bool>(_::Private::is_meta_object<GadgetObject*>::value),   true);
    QCOMPARE(static_cast<bool>(_::Private::is_meta_object<GadgetObject**>::value),  false);

    QCOMPARE(static_cast<bool>(_::Private::is_meta_object<QString>::value), false);
}

void C11TestCases::test_private_is_qobject()
{
    QCOMPARE(static_cast<bool>(_::Private::is_qobject<QObject>::value),          true);
    QCOMPARE(static_cast<bool>(_::Private::is_qobject<QObject*>::value),         true);
    QCOMPARE(static_cast<bool>(_::Private::is_qobject<QObject*&>::value),        true);
    QCOMPARE(static_cast<bool>(_::Private::is_qobject<const QObject*>::value),   true);
    QCOMPARE(static_cast<bool>(_::Private::is_qobject<C11TestCases>::value),     true);
    QCOMPARE(static_cast<bool>(_::Private::is_qobject<C11TestCases*>::value),    true);
    QCOMPARE(static_cast<bool>(_::Private::is_qobject<GadgetObject>::value),     false);
}

void C11TestCases::test_private_is_key_value_type()
{
    QCOMPARE(static_cast<bool>(_::Private::is_key_value_type<QObject>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::is_key_value_type<QObject*>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::is_key_value_type<C11TestCases>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::is_key_value_type<C11TestCases*>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::is_key_value_type<GadgetObject>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::is_key_value_type<QVariantMap>::value), true);
    QCOMPARE(static_cast<bool>(_::Private::is_key_value_type<int>::value), false);
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

    QCOMPARE(static_cast<int> (_::Private::is_args_compatible<decltype(myFunc0)>::value), 1);
    QCOMPARE(static_cast<int> (_::Private::is_args_compatible<decltype(myFunc0), int>::value), 0);

    QCOMPARE(static_cast<int> (_::Private::is_args_compatible<decltype(myFunc1),int>::value), 1);
    QCOMPARE(static_cast<int> (_::Private::is_args_compatible<decltype(myFunc1),int>::value), 1);
    QCOMPARE(static_cast<int> (_::Private::is_args_compatible<decltype(myFunc1),QString, int>::value), 0);
    QCOMPARE(static_cast<int> (_::Private::is_args_compatible<decltype(myFunc1),QString, int>::value), 0);

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

    QCOMPARE(static_cast<bool> (_::Private::is_invokable3<decltype(myFunc2), int, int, int>::value), true);
    QCOMPARE(static_cast<bool> (_::Private::is_invokable3<decltype(myFunc2), int, QString, int>::value), false);
}

void C11TestCases::test_private_invoke_by_read()
{
    {
        /* QMap */
        QMap<QString, int> map{{"value1", 1}};

        static_assert(_::Private::is_kyt_key_matched<QMap<QString,int>, const char*>::value, "");
        static_assert(_::Private::is_readable<QMap<QString,int>, const char*>::value, "");
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

    QCOMPARE(static_cast<bool> (_::Private::has_static_meta_object<A>::value), false);
    QCOMPARE(static_cast<bool> (_::Private::has_static_meta_object<DataObject*>::value), true);
    QCOMPARE(static_cast<bool> (_::Private::has_static_meta_object<GadgetObject>::value), true);
    QCOMPARE(static_cast<bool> (_::Private::has_static_meta_object<GadgetObject*>::value), true);


    QCOMPARE(static_cast<bool> (_::Private::has_static_meta_object<DummyClassWithStaticMetaObject>::value), false);

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
    QCOMPARE((_::Private::key_value_read(&gadget, "value")), QVariant(10));
    QCOMPARE((_::Private::key_value_read(gadget, "value")), QVariant(10));
    QCOMPARE((_::Private::key_value_read(gadget, "value1")), QVariant());

    QVERIFY((_::Private::is_kyt_key_matched<decltype(gadget), decltype("value")>::value));
    QVERIFY(!(_::Private::is_collection_index_matched<decltype(gadget), decltype("value")>::value));

    QCOMPARE((_::Private::read(&gadget, "value")), QVariant(10));
    QCOMPARE((_::Private::read(gadget, "value")), QVariant(10));
    QCOMPARE((_::Private::read(gadget, "value1")), QVariant());

    QObject* object = new QObject(this);
    object->setObjectName("objectName");
    QCOMPARE((_::Private::key_value_read(object, "objectName")), QVariant("objectName"));
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

        auto ptr = &object;
        _::Private::write(ptr, "value", 2);
        QCOMPARE(object.value, 2);

    }

    {
        auto  object = new DataObject(this);

        object->setValue1(0);
        _::Private::write(object, "value1", 1);
        QCOMPARE(object->value1(), 1);
    }
}

void C11TestCases::test_private_cast_to_pointer()
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

void C11TestCases::test_private_cast_to_qobject()
{
    QObject* object = new QObject(this);
    const QObject* const_object = object;

    QCOMPARE(_::Private::cast_to_qobject(object), object);
    QCOMPARE(_::Private::cast_to_qobject(const_object), const_object);

    QVERIFY((std::is_same<decltype(_::Private::cast_to_qobject(object)), QObject*>::value) > 0);
    QVERIFY((std::is_same<decltype(_::Private::cast_to_qobject(const_object)), const QObject*>::value) > 0);

}

void C11TestCases::test_private_merge()
{
    {
        /* QVariant, QObject */
        QVariant v1;
        DataObject* v2 = new DataObject(this);
        v2->setValue1(1);

        auto res = _::Private::p_merge_(v1, v2);
        auto map = res.toMap();
        QCOMPARE(map["value1"].toInt(), 1);
    }

    {
        /* QVariant , QVariant(QObject) */
        QVariant v1;
        DataObject* object = new DataObject(this);
        object->setValue1(1);

        QVariant v2 = QVariant::fromValue<DataObject*>(object);

        auto res = _::Private::p_merge_(v1, v2);
        auto map = res.toMap();
        QCOMPARE(map["value1"].toInt(), 1);
    }

    {
        /* QObject, QVariant */
        DataObject* v1 = new DataObject(this);
        v1->setValue1(1);

        QVariantMap v2;
        v2["value1"] = 10;

        auto res = _::Private::p_merge_(v1, v2);

        QVERIFY(v1 == res);
        QCOMPARE(v1->value1(), 10);
    }
}

void C11TestCases::test_private_GadgetContainer()
{
    {   /* Qt API Behaviour Validation */
        QVariant v;
        v = 40;

        QMetaType intType(v.userType());
        QVERIFY(intType.metaObject() == nullptr);

        GadgetObject object;
        object.value = 13;
        v.setValue<GadgetObject>(object);

        int indexOfProperty = GadgetObject::staticMetaObject.indexOfProperty("value");

        QMetaProperty property = GadgetObject::staticMetaObject.property(indexOfProperty);
        QVariant propertyValue = property.readOnGadget(v.data());

        /* Warning: QVariant::data() is a non-documented API and not used by any inline functions */

        QCOMPARE(propertyValue.toInt(), 13);

        QMetaType type(v.userType());

        QCOMPARE(type.metaObject() != nullptr, true);

        const QMetaObject* metaObject = type.metaObject();

        QCOMPARE(QString(metaObject->className()), QString("GadgetObject"));
    }

    {
        /* Initialization */
        _::Private::GadgetContainer gadget;

        QCOMPARE(static_cast<bool>(_::Private::is_meta_object<_::Private::GadgetContainer>::value), true);
        QCOMPARE(_::isKeyValueType(gadget), true);
        QVERIFY(gadget.data == nullptr);
        QVERIFY(gadget.metaObject == nullptr);
    }

    {
        /* non-gadget */
        QVariant v;
        QObject* obj = new QObject(this);
        v.setValue<QObject*>(obj);
        _::Private::GadgetContainer gadget = _::Private::cast_to_gadget_container(v);
        QVERIFY(gadget.data == nullptr);
        QVERIFY(gadget.constData == nullptr);
        QVERIFY(gadget.metaObject == nullptr);
    }

    {
        /* Const */
        QVariant v;
        GadgetObject object;
        object.value = 13;
        v.setValue<GadgetObject>(object);
        const QVariant& cv = v;
        _::Private::GadgetContainer gadget = _::Private::cast_to_gadget_container(cv);
        QVERIFY(gadget.data == nullptr);
        QVERIFY(gadget.constData != nullptr);
        QVERIFY(gadget.metaObject == &GadgetObject::staticMetaObject);

        QCOMPARE(_::Private::read(gadget, "value").toInt(), 13);

        QStringList keys;
        _::forIn(gadget, [&](const QVariant&, const QString& key) {
            keys << key;
        });
        QCOMPARE(keys, QStringList{"value"});
    }

    {
        /* Non-const */
        QVariant v;
        GadgetObject object;
        object.value = 13;
        v.setValue<GadgetObject>(object);
        _::Private::GadgetContainer gadget = _::Private::cast_to_gadget_container(v);
        QVERIFY(gadget.data != nullptr);
        QVERIFY(gadget.constData != nullptr);
        QVERIFY(gadget.metaObject == &GadgetObject::staticMetaObject);

        QCOMPARE(_::Private::read(gadget, "value").toInt(), 13);

        _::Private::write(gadget, "value", "14");

        object = v.value<GadgetObject>();
        QCOMPARE(object.value, 14);

        QStringList keys;
        _::forIn(gadget, [&](const QVariant&, const QString& key) {
            keys << key;
        });
        QCOMPARE(keys, QStringList{"value"});
    }

}

void C11TestCases::test_private_contains()
{
    QCOMPARE(_::Private::contains(10, "123"), false);

    QCOMPARE(_::Private::contains(std::map<std::string, int>{{"v1", 1} }, "v1"), true);
    QCOMPARE(_::Private::contains(std::map<std::string, int>{{"v1", 1} }, "v2"), false);


    QCOMPARE(_::Private::contains(new DataObject(this), "value1"), true);
    QCOMPARE(_::Private::contains(new DataObject(this), "valueX"), false);

    GadgetObject gadget;

    QCOMPARE(_::Private::contains(gadget, "value"), true);
    QCOMPARE(_::Private::contains(gadget, "valueX"), false);

}

void C11TestCases::test_merge()
{

    {
        /* QVariantMap, QObject */

        QObject* source = createMockObject(this);
        QVariantMap object;
        _::merge(object, source);

        QVERIFY(object["objectName"] == "Root");
        QVERIFY(object["value1"].toInt() == 1);
        QVERIFY(object["value2"].toString() == "2");
        QVERIFY(object["value3"].toBool());

        QVERIFY(object["value4"].type() == QVariant::Map);
        QVERIFY(object["value4"].toMap()["value1"].toInt() == 5);
    }

    {
        /* QVariantMap, QVariantMap */

        QVariantMap source{{"value1", 1},{"value2", 2.0}};
        QVariantMap object{{"value3", "3"}};

        _::assign(object, source);

        QCOMPARE(object.size(), 3);
        QCOMPARE(object["value1"].toInt(), 1);
        QCOMPARE(object["value2"].toDouble(), 2.0);
        QCOMPARE(object["value3"].toString(), QString("3"));
    }
}

void C11TestCases::test_merge_qobject()
{

    {
        /* QObject, QObject */
        auto* source = createMockObject(this);
        auto* dest = createMockObject(this);

        source->setProperty("value3", "3+");
        source->value4()->setValue1(33);

        _::merge(dest, source);

        QCOMPARE(dest->property("value3").toString(), QString("3+"));

        QVERIFY(dest->property("value4").value<QObject*>() != source->property("value4").value<QObject*>());
        QCOMPARE(dest->value4()->value1(), 33);
    }

    {
         /* QObject, QVariantMap */
        QObject* object = createMockObject(this);

        QVariantMap value4{{"value1", 32}};
        _::merge(object, QVariantMap{{"value1", 99}, {"value4", value4}});
        QCOMPARE(object->property("value1").toInt(), 99);
        QCOMPARE(object->property("value4").value<QObject*>()->property("value1").toInt(),32);
    }

    {
         /* QObject, std::map */
        QObject* object = createMockObject(this);

        std::map<QString,int> source{{"value1", 32}};
        _::merge(object, source);
        QVERIFY(object->property("value1").toInt() == 32);
    }
}

void C11TestCases::test_merge_gadget()
{
    {
        /* QVariantMap, Gadget */

        GadgetObject source;
        source.value = 33;

        QVariantMap object;

        auto res = _::merge(object, source);

        QCOMPARE(res, object);
        QCOMPARE(object["value"].toInt(), 33);
    }

    {
        /* Gadget, QVariantMap */
        QVariantMap source;
        source["value"] = 44;
        source["other"] = 55;

        GadgetObject object;

        QCOMPARE(_::merge(object, source).value, 44);
    }

    {
        /* QVariantMap{Gadget}, QVariantMap */

        QVariantMap source;
        source["gadget"] = QVariantMap{{"value", 34}, {"other", 55}};

        GadgetObject gadget;

        QVariantMap object;
        object["gadget"] = QVariant::fromValue<GadgetObject>(gadget);

        _::merge(object, source);

        gadget = object["gadget"].value<GadgetObject>();

        QCOMPARE(gadget.value, 34);
    }

    {
        /* QVariantMap, QVariantMap{Gadget}*/

        GadgetObject gadget;
        gadget.value = 47;
        QVariantMap source;
        source["gadget"] = QVariant::fromValue<GadgetObject>(gadget);

        QVariantMap object;
        _::merge(object, source);

        QVERIFY(object.contains("gadget"));
        QCOMPARE(object["gadget"].toMap()["value"].toInt(), 47);
    }

}

void C11TestCases::test_merge_complex()
{
    {
        QVariantMap object;
        ComplexQObject* source = new ComplexQObject(this);

        _::merge(object, source);

        QVariantMap value1 = object["value1"].toMap();
        QCOMPARE(value1["value"].toInt(), 11);

        QVariantMap value2 = object["value2"].toMap();
        QCOMPARE(value2["value3"].toString(), QString("23.0"));

        QVariantMap value2value4 = value2["value4"].toMap();
        QCOMPARE(value2value4["value1"].toInt(), 5);
    }

    {
        ComplexQObject* object = new ComplexQObject(this);

        QVariantMap source;
        source["value1"] = QVariantMap{{"value", 33}, {"other", 4}};
        source["value2"] = QVariantMap{{"value1", 41}, {"value4", QVariantMap{{"value1", 57}} }};
        source["value3"] = QVariantMap{{"value1", 70}};
        source["value4"] = QVariantList{1,2,3};

        _::merge(object, source);

        QCOMPARE(object->value1().value, 33);
        QCOMPARE(object->value2()->value1(), 41);
        QCOMPARE(object->value2()->value4()->value1(), 57);
        QCOMPARE(object->value3()["value1"].toInt(), 70);
    }

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

void C11TestCases::test_rangeQ()
{
    QCOMPARE(_::rangeQ(4), (QList<int>{0,1,2,3}));

    QCOMPARE(_::rangeQ(-4), (QList<int>{0,-1,-2,-3}));
    QCOMPARE(_::rangeQ(1,5), (QList<int>{1,2,3,4}));
    QCOMPARE(_::rangeQ(0, 20, 5), (QList<int>{0, 5, 10, 15}));
    QCOMPARE(_::rangeQ(0, 20, 5.0), (QList<int>{0, 5, 10, 15}));
    QCOMPARE(_::rangeQ(1,4,0), (QList<int>{1,1,1}));
    QCOMPARE(_::rangeQ(0), (QList<int>{}));
}

void C11TestCases::test_isMap()
{
    QCOMPARE(_::isMap(QMap<int,int>{}),       true);
    QCOMPARE(_::isMap(QVariantMap{}),         true);

    QCOMPARE(_::isMap(std::vector<int>{}),    false);
    QCOMPARE(_::isMap(QVector<int>{ }),       false);
    QCOMPARE(_::isMap(QList<int>{ }),         false);
    QCOMPARE(_::isMap(QVariantList{ }),       false);
    QCOMPARE(_::isMap(QString{ }),            false);
    QCOMPARE(_::isMap(10),                    false);
    QCOMPARE(_::isMap(new C11TestCases(this)),false);
}

void C11TestCases::test_isKeyValueType()
{
    QCOMPARE(_::isKeyValueType(QMap<int,int>{}),       true);
    QCOMPARE(_::isKeyValueType(QVariantMap{}),         true);
    QCOMPARE(_::isKeyValueType(new QObject(this)),     true);
    QCOMPARE(_::isKeyValueType(std::map<int,int>{}),   true);

    QCOMPARE(_::isKeyValueType(GadgetObject()),        true);

    QCOMPARE(_::isKeyValueType(std::vector<int>{}),    false);
    QCOMPARE(_::isKeyValueType(QVector<int>{ }),       false);
    QCOMPARE(_::isKeyValueType(QList<int>{ }),         false);
    QCOMPARE(_::isKeyValueType(QVariantList{ }),       false);
    QCOMPARE(_::isKeyValueType(QString{ }),            false);
    QCOMPARE(_::isKeyValueType(10),                    false);
}

void C11TestCases::test_isQtMetable()
{
    QCOMPARE(_::isQtMetable(QMap<int,int>{}),       true);
    QCOMPARE(_::isQtMetable(QVariantMap{}),         true);
    QCOMPARE(_::isQtMetable(new QObject(this)),     true);
    QCOMPARE(_::isQtMetable(GadgetObject()),        true);

    QCOMPARE(_::isQtMetable(std::vector<int>{}),    false);
    QCOMPARE(_::isQtMetable(QVector<int>{ }),       false);
    QCOMPARE(_::isQtMetable(QList<int>{ }),         false);
    QCOMPARE(_::isQtMetable(QVariantList{ }),       false);
    QCOMPARE(_::isQtMetable(QString{ }),            false);
    QCOMPARE(_::isQtMetable(10),                    false);
    QCOMPARE(_::isQtMetable(std::map<int,int>{}),   false);
}



