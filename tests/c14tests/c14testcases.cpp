#include <QQmlApplicationEngine>
#include <QTest>
#include <Automator>
#include <QtShell>
#include <functional>
#include <vector>
#include "c14testcases.h"
#include "underline.h"
#include "dataobject.h"
#include "gadgetobject.h"

static bool isOdd(int value) {
    return value % 2 == 1;
}

static QObject* createMockObject(QObject* parent) {
    QObject* ret = new DataObject(parent);
    ret->setProperty("value1", 1);
    ret->setProperty("value2", 2.0);
    ret->setProperty("value3", "3");

    return ret;
}

C14TestCases::C14TestCases(QObject *parent) : QObject(parent)
{
    auto ref = [=]() {
        QTest::qExec(this, 0, 0); // Autotest detect available test cases of a QObject by looking for "QTest::qExec" in source code
    };
    Q_UNUSED(ref);
}

void C14TestCases::test_private_is_qobject()
{
    QCOMPARE((bool) _::Private::is_qobject<C14TestCases>::value, true);
    QCOMPARE((bool) _::Private::is_qobject<GadgetObject>::value, false);

    QCOMPARE((bool) _::Private::is_gadget<C14TestCases>::value, false);
    QCOMPARE((bool) _::Private::is_gadget<GadgetObject>::value, true);

}

template <typename F, typename T>
auto wrapper(F functor, T t) -> typename _::Private::ret_func<F,T>::type {
    return functor(t);
}

void C14TestCases::test_private_traits()
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

void C14TestCases::test_private_invoke()
{
    auto func0 = []() {
        return -1;
    };

    auto func1 = [](auto i) {
        return i;
    };

    auto func2 = [](auto i, int j) {
        Q_UNUSED(j);
        return i;
    };

    auto func2_intToString = [](auto i, int j) {
        Q_UNUSED(j);
        return i.toInt();
    };

    auto func3 = [](auto i, auto j, auto k) {
        Q_UNUSED(j);
        Q_UNUSED(k);
        return i;
    };

    QCOMPARE((int) (_::Private::is_args_compatible<decltype(func0)>::value), 1);
    QCOMPARE((int) (_::Private::is_args_compatible<decltype(func0), int>::value), 0);

    QCOMPARE((int) (_::Private::is_args_compatible<decltype(func1),int>::value), 1);
    QCOMPARE((int) (_::Private::is_args_compatible<decltype(func1),QString>::value), 1);
    QCOMPARE((int) (_::Private::is_args_compatible<decltype(func1),QString, int>::value), 0);
    QCOMPARE((int) (_::Private::is_args_compatible<decltype(func1),QString, int>::value), 0);

    QCOMPARE((std::is_same<decltype(_::Private::decl_func0<decltype(func0)>()), int>::value), true);

    QCOMPARE((std::is_same<decltype(_::Private::decl_func0<decltype(func1),QString>()), QString>::value), true);

    QCOMPARE((std::is_same<_::Private::ret_func<decltype(func0)>::type,int>::value), true);

    QCOMPARE((std::is_same<_::Private::ret_func<decltype(func1),int>::type,int>::value), true);
    QCOMPARE((std::is_same<_::Private::ret_func<decltype(func2),int, int>::type,int>::value), true);
    QCOMPARE((std::is_same<_::Private::ret_func<decltype(func2),QString, int>::type,QString>::value), true);
    QCOMPARE((std::is_same<_::Private::ret_func<decltype(func2_intToString),QString, int>::type , int>::value), true);

    QCOMPARE(_::Private::invoke(func0) , -1);
    QCOMPARE(_::Private::invoke(func0, 1) , -1);
    QCOMPARE(_::Private::invoke(func0, 2,3) , -1);

    QCOMPARE(_::Private::invoke(func1, 1) , 1);
    QCOMPARE(_::Private::invoke(func1, 2,3) , 2);

    QCOMPARE(_::Private::invoke(func2, 2,3) , 2);

    QCOMPARE(_::Private::invoke(func3, 3,4,5) , 3);
    QCOMPARE(_::Private::invoke(func2, 3,4,5) , 3);
    QCOMPARE(_::Private::invoke(func1, 3,4,5) , 3);
    QCOMPARE(_::Private::invoke(func0, 3,4,5) , -1);

    QCOMPARE((std::is_same<decltype(_::Private::decl_invoke0<decltype(func0)>()) , int>::value), true);

    QCOMPARE((std::is_same<decltype(_::Private::decl_invoke0<decltype(func2), QString, int>()) , QString>::value), true);

    QCOMPARE((std::is_same<_::Private::ret_invoke<decltype(func0),QString, int>::type , int>::value), true);
}

void C14TestCases::test_private_value()
{

    {
        auto voidFunc = []() {
        };

        _::Private::Value<void> value;
        value.invoke(voidFunc);
        QCOMPARE(value.canConvert<bool>(), false);
        QCOMPARE(value.canConvert<QString>(), false);

        QCOMPARE(value.equals(true), false);
        QCOMPARE(value.equals(false), false);
    }

    {
        _::Private::Value<bool> value;
        value.invoke(isOdd, 1);
        QCOMPARE((std::is_convertible<bool, bool>::value), true);
        QCOMPARE(value.canConvert<bool>(), true);
        QCOMPARE(value.canConvert<QString>(), false);

        QCOMPARE(value.equals(true), true);
        QCOMPARE(value.equals(false), false);
        QCOMPARE(value.equals(QString("")), false);

        value.invoke(isOdd, 0);
        QCOMPARE(value.equals(true), false);
        QCOMPARE(value.equals(false), true);
    }
}

void C14TestCases::test_some()
{
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
        auto func = [](auto item, auto index, auto collection) {
            Q_UNUSED(index);
            Q_UNUSED(collection);
            return item % 2 == 1;
        };

        QCOMPARE(_::some(QList<int>() << 0 << 2 << 4, func), false);
        QCOMPARE(_::some(QList<int>() << 0 << 3 << 4, func), true);
    }
}

void C14TestCases::test_map()
{
    {
        auto func = [](QString item) {
            return item.toInt();
        };

        QCOMPARE(_::map(QList<QString>() << "1" << "2" << "3", func), QList<int>() << 1 << 2 << 3);
        QCOMPARE(_::map(QVector<QString>() << "1" << "2" << "3", func), QVector<int>() << 1 << 2 << 3);
    }

    {
        // generic lambda
        auto func = [](auto item) {
            return item.toInt();
        };

        QCOMPARE(_::map(QList<QString>() << "1" << "2" << "3", func), QList<int>() << 1 << 2 << 3);
        QCOMPARE(_::map(QVector<QString>() << "1" << "2" << "3", func), QVector<int>() << 1 << 2 << 3);
    }

    {
        // generic lambda with index parameter
        auto func = [](auto item, int index) {
            Q_UNUSED(index);
            return item.toInt();
        };

        QCOMPARE(_::map(QStringList() << "1" << "2" << "3", func), QList<int>() << 1 << 2 << 3);
        QCOMPARE(_::map(QVector<QString>() << "1" << "2" << "3", func), QVector<int>() << 1 << 2 << 3);
    }

    {
        // generic lambda with index and collection parameter
        auto func = [](auto item, int index, auto collection) {
            Q_UNUSED(index);
            Q_UNUSED(collection);
            return item.toInt();
        };

        QCOMPARE(_::map(QStringList() << "1" << "2" << "3", func), QList<int>() << 1 << 2 << 3);
        QCOMPARE(_::map(QVector<QString>() << "1" << "2" << "3", func), QVector<int>() << 1 << 2 << 3);
    }

    {
        auto func = [](QString item, int index, auto collection) {
            Q_UNUSED(index);
            Q_UNUSED(collection);
            return item.toInt();
        };

        std::vector<QString> l1{"1", "2", "3"};
        std::vector<int> l2{1, 2, 3};

        QCOMPARE((_::map(l1, func)), l2);
    }
}

void C14TestCases::test_assign()
{
    QObject* root = createMockObject(this);

    QVERIFY(root);

    /* assign(map, QObject) */

    {
        QVariantMap data;
        _::assign(data, root);

        QVERIFY(data["objectName"] == "Root");
        QVERIFY(data["value1"].toInt() == 1);
        QVERIFY(data["value2"].toString() == "2");
        QVERIFY(data["value3"].toBool());

        QVERIFY(data["value4"].canConvert<QObject*>());
    }


    {
        /* assign(QObject, map) */

        QVariantMap value4{{"value1", 32}};
        _::assign(root, QVariantMap{{"value1", 99}, {"value4", value4}});
        QVERIFY(root->property("value1").toInt() == 99);
    }

    {
        /* assign(map, map) */

        QVariantMap source{{"value1", 1},{"value2", 2.0}};
        QVariantMap dest{{"value3", "3"}};

        _::assign(dest, source);

        QCOMPARE(dest.size(), 3);
        QCOMPARE(dest["value1"].toInt(), 1);
        QCOMPARE(dest["value2"].toDouble(), 2.0);
        QCOMPARE(dest["value3"].toString(), QString("3"));
    }

    {
        /* assign(object, object) */
        QObject* source = createMockObject(this);
        QObject* dest = createMockObject(this);

        source->setProperty("value3", "3+");
        _::assign(dest, source);

        QCOMPARE(dest->property("value3").toString(), QString("3+"));

        QCOMPARE(dest->property("value4").value<QObject*>(), source->property("value4").value<QObject*>());
    }

    {
        QVariantMap s1{{"valueA", 1}, {"valueB", 2.0}};
        QVariantMap s2{{"valueC", "3"}, {"valueD", true}};
        QObject* object = createMockObject(this);

        QVariantMap output;
        _::assign(output, s1, s2, object);
        QCOMPARE(output.size(), 9);
        QCOMPARE(output["objectName"].toString(), QString("Root"));
        QCOMPARE(output["valueD"].toBool(), true);
        QCOMPARE(output["value1"].toInt(), 1);
    }
}

void C14TestCases::test_get()
{

    /* get(QObject*, QString) */

    {
        QObject* root = createMockObject(this);
        QVERIFY(root);

        QVariant value = _::get(root, "value4.value1");
        QCOMPARE(value.toInt(), 5);

        value = _::get(root,"value4.valueX", QString("Not Found"));
        QVERIFY(value.toString() == "Not Found");
    }

    /* get(QVarnaintMap, QString) */

    {
        QObject* root = createMockObject(this);
        QVERIFY(root);

        QVariantMap source;
        _::assign(source, root);
        auto value = _::get(source, "value3");
        QCOMPARE(value.toString(), QString("3"));

        value = _::get(source, "valueX");
        QVERIFY(value.isNull());
    }
}

void C14TestCases::test_set()
{
    QVariantMap data;
    _::set(data,"value1", 1);
    QVERIFY(data.contains("value1"));
    QVERIFY(data["value1"].toInt() == 1);

    _::set(data,"value2","value2");
    QVERIFY(data.contains("value1"));
    QVERIFY(data["value1"].toInt() == 1);
    QVERIFY(data.contains("value2"));
    QVERIFY(data["value2"].toString() == "value2");

    _::set(data,"value3.value1",2);

    QVERIFY(data.contains("value3"));
    QVariantMap value3 = data["value3"].toMap();
    QVERIFY(value3["value1"].toInt() == 2);

    /* Override value */
    data["value4"] = true;
    _::set(data,"value4.value1",3);
    QVariant value4 = data["value4"];
    QVERIFY(value4.canConvert<QVariantMap>());

    QVERIFY(value4.toMap()["value1"].toInt() == 3);
}

void C14TestCases::test_pick()
{
    QObject* root = createMockObject(this);
    QVERIFY(root);

    /* _::pick(QObject*, paths) */

    QVariantMap data = _::pick(root, QStringList()
                                       << "value1"
                                       << "value4.value1");

    QCOMPARE(data.size(), 2);
    QVERIFY(data.contains("value1"));
    QVERIFY(!data.contains("value2"));
    QVERIFY(data.contains("value4"));

    QVERIFY(data["value4"].toMap()["value1"].toInt() == 5);

    // Pick an QObject
    data = _::pick(root, QStringList() << "value4");
    QVERIFY(data["value4"].type() == QVariant::Map);

    /* _::pick(QVariant, paths) */
    QVariantMap source;
    _::assign(source, root);

    data = _::pick(source, QStringList() << "value1" << "value4.value1");

    QCOMPARE(data.size(), 2);
    QVERIFY(data.contains("value1"));
    QVERIFY(!data.contains("value2"));
    QVERIFY(data.contains("value4"));

    QVERIFY(data["value4"].toMap()["value1"].toInt() == 5);

}

void C14TestCases::test_omit()
{
    QObject* root = createMockObject(this);

    QVERIFY(root);

    QVariantMap data1;

    _::assign(data1, root);
    QCOMPARE(data1.contains("value2"), true);

    QVariantMap properties;
    properties["value1"] = true;
    properties["value3"] = false; // omit do not care the content

    QVariantMap data2 = _::omit(data1, properties);

    QVERIFY(!data2.contains("value1"));
    QVERIFY(data2.contains("value2"));
    QVERIFY(!data2.contains("value3"));
    QVERIFY(data2.contains("value4"));
    QVERIFY(data2["value4"].type() == QVariant::Map);
}

void C14TestCases::test_forIn()
{
    {
        /* QVariantMap */
        QVariantMap data{{"value1", 1}, {"value2", 2.0}};
        QList<QString> keys;
        QVariantList values;

        auto func = [&](auto value, auto key, auto collection) -> void {
            Q_UNUSED(collection);
            values << value;
            keys << key;
        };

        QCOMPARE((std::is_same<_::Private::ret_invoke<decltype(func), QVariant, QString, QVariantMap>::type, void>::value), true);

        _::Private::Value<_::Private::ret_invoke<decltype(func), QVariant, QString, QVariantMap>> value;
        QCOMPARE(value.canConvert<bool>(), false);

        _::forIn(data, func);

        QCOMPARE(keys, data.keys());
        QCOMPARE(values, data.values());
    }

    {
        /* QVariantMap and Early Termination */
        QVariantMap data{{"value1", 1}, {"value2", 2.0}};
        QList<QString> keys;
        QVariantList values;
        _::forIn(data, [&](auto value, auto key) {
            values << value;
            keys << key;
            return false;
        });

        QCOMPARE(keys.size(), 1);
        QCOMPARE(values.size(), 1);
    }

    {
        // _.forIn(QObject*)
        DataObject* object = new DataObject(this);
        bool error = false;
        QVariantList proeprties;

        _::forIn(object, [&](auto value, QString key, auto object) {
            const QMetaObject* meta = object->metaObject();
            if (meta->indexOfProperty(key.toUtf8().constData()) < 0) {
                error = true;
            }
            proeprties << value;
        });
        QCOMPARE(error, false);
        QCOMPARE(proeprties.size(), 5);
    }

    {
        // _.forIn(QObject*) and early termination
        DataObject* object = new DataObject(this);
        bool error = false;
        QVariantList proeprties;

        _::forIn(object, [&](auto value, QString key, auto object) {
            const QMetaObject* meta = object->metaObject();
            if (meta->indexOfProperty(key.toUtf8().constData()) < 0) {
                error = true;
            }
            proeprties << value;
            return false;
        });

        QCOMPARE(error, false);
        QCOMPARE(proeprties.size(), 1);
    }

    {
        GadgetObject object;
        QVariantList keys;

        _::forIn(&object, [&](auto, auto key) {
            keys << key;
        });

        QCOMPARE(keys.size(), 1);
    }

    {
        GadgetObject object;
        QVariantList keys;

        _::forIn(object, [&](auto, auto key) {
            keys << key;
        });

        QCOMPARE(keys.size(), 1);
    }

}

void C14TestCases::test_forEach()
{
    {
        int count = 0;
        _::forEach(QStringList{"1","2","3"}, [&]() {count++;});
        QCOMPARE(count, 3);
    }

    {
        int count = 0;
        _::forEach(std::vector<QString>{"1","2","3"}, [&](auto value) { if (value == "2") count++;});
        QCOMPARE(count, 1);
    }

    {
        int count = 0;
        _::forEach(QList<QString>{"1","2","3"}, [&](auto, int index) { if (index > 0) count++;});
        QCOMPARE(count, 2);
    }

    {
        int count = 0;
        _::forEach(QVector<QString>{"1","2","3"}, [&](auto, int index, auto collection) { if (collection[index].toInt() > 1) count++;});
        QCOMPARE(count, 2);
    }

    {
        int count = 0;
        _::forEach(QStringList{"1","2","3"}, [&](auto, int , auto ) { count++; return false;});
        QCOMPARE(count, 1);
    }
}

void C14TestCases::test_reduce()
{
    {
        auto value = _::reduce(std::vector<int>{1,2,3}, [](auto acc, auto value) {
            return acc + value;
        },0);

        QCOMPARE(value, 6);
    }

    {
        auto value = _::reduce(QList<int>{1,2,3}, [](auto acc, auto value, auto index, auto) {
            return acc + value + index;
        }, 0);

        QCOMPARE(value, 9);
    }
}

void C14TestCases::test_countBy()
{
    QCOMPARE(_::countBy(_::range<std::vector<int>>(5), [](auto value) {return value % 2 == 0 ? std::string("even") : std::string("odd"); })["even"] , 3);

    QCOMPARE(_::countBy(_::range<std::vector<int>>(5), [](auto value) {return value % 2 == 0 ? "even" : "odd"; })["odd"] , 2);

    {
        auto input = _::range<QList<int>>(5);
        QCOMPARE(_::countBy(input, [](auto value) {return value % 2 == 0 ? "even" : "odd"; })["odd"] , 2);
    }

    {
        auto input = _::map(_::range_q(5) , [](auto item) {
            QMap<QString, bool> map;
            map["isOdd"] = item % 2 == 1;
            return map;
        });

        QCOMPARE(_::countBy(input,"isOdd")[true] , 2);
        QCOMPARE(input.size(), 5);
    }

    {
        auto worker = [](const auto input) {
            return _::countBy(input, [](auto value) {return value % 2 == 0 ? "even" : "odd"; });
        };

        QCOMPARE(worker(_::range_q(5))["odd"], 2);
    }
}

void C14TestCases::test_isArray()
{
    QCOMPARE(_::isArray(std::vector<int>{}),    true);
    QCOMPARE(_::isArray(QVector<int>{ }),       true);
    QCOMPARE(_::isArray(QList<int>{ }),         true);
    QCOMPARE(_::isArray(QVariantList{ }),       true);
    QCOMPARE(_::isArray(QString{ }),            true);

    QCOMPARE(_::isArray(std::map<bool,int>{}),  false);
    QCOMPARE(_::isArray(QMap<int,int>{}),       false);
    QCOMPARE(_::isArray(10),                    false);

}

void C14TestCases::test_assignWith()
{
    {
        QVariantMap object{{"value1",1}};
        std::map<QString,double> source{{"value2", 2.0}};

        QVERIFY((std::is_same<QString, typename _::Private::key_value_type<QVariantMap>::key_type>::value));
        QVERIFY((std::is_same<std::string, _::Private::key_value_type<std::map<std::string,double>>::key_type>::value));

        _::assignWith(object, source, [](auto , auto srcValue, auto , auto , auto ) -> double {
            return srcValue + 1;
        });

        QCOMPARE(object["value1"].toInt(), 1);
        QCOMPARE(object["value2"].toDouble(), 3.0);
    }

}
