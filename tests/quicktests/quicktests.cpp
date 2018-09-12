#include <QTest>
#include <QQmlApplicationEngine>
#include <QtShell>
#include "quicktests.h"
#include "quicktests.h"

#define _underline_debug(x) { qDebug() << x;}
#include <underline.h>

QuickTests::QuickTests(QObject *parent) : QObject(parent)
{
    auto ref = [=]() {
        QTest::qExec(this, 0, nullptr); // Autotest detect available test cases of a QObject by looking for "QTest::qExec" in source code
    };
    Q_UNUSED(ref);
}

void QuickTests::validate_QJSValue_expected_behaviour()
{
    {   // Ccreate non-Object value without using QQmlEngine
        QJSValue intValue(1);
        QCOMPARE(intValue.toInt(), 1);
    }

    {
        // Create object by prototype
        QQmlEngine engine;

        QJSValue object = engine.newObject();
        QCOMPARE(object.isObject(), true);

        QJSValue prototype = object.prototype();

        QJSValue newObject = prototype.property("constructor").callAsConstructor();

        QCOMPARE(newObject.isObject(), true);
    }

    {
        // Prove it is able to obtain Object.prototype from QJSValue
        QQmlEngine engine;

        QJSValue objectPrototype = engine.newObject().prototype();

        QString content = QtShell::cat(QtShell::realpath_strip(SRCDIR, "test_QJSValue.js"));
        QJSValue program = engine.evaluate(content);

        QVERIFY(!program.isError());

        QJSValue testObjectCreator = program.property("TestObject");
        QVERIFY(testObjectCreator.isCallable());

        QJSValue testObject = testObjectCreator.callAsConstructor();
        QCOMPARE(testObject.isObject(), true);
        QCOMPARE(testObject.property("value").toInt(), 50);

        QJSValue prototype = testObject.prototype();

        QJSValue object1 = prototype.property("constructor").callAsConstructor();
        QCOMPARE(object1.property("value").toInt(), 50);

        prototype = prototype.prototype();
        QJSValue object2 = prototype.property("constructor").callAsConstructor();
        QVERIFY(object2.property("value").toInt() != 50);
        QVERIFY(object2.isObject());

        QVERIFY(objectPrototype.equals(prototype));

        QVERIFY(prototype.prototype().isNull());
    }
}

void QuickTests::spec_QJSValue()
{
    QCOMPARE(static_cast<bool>(_::Private::is_meta_object<QJSValue>::value),         true);
    QCOMPARE(static_cast<bool>(_::Private::is_qt_any_type<QJSValue>::value),         true);
    QCOMPARE(static_cast<bool>(_::isQtMetable<QJSValue>()),                          true);

    QCOMPARE(static_cast<bool>(_::Private::is_real_key_value_type<QJSValue>::value), false);

    QJSEngine engine;
    QJSValue value = engine.toScriptValue(QVariantMap{});
    _::Private::write(value, "value1", 1);

    QCOMPARE(value.property("value1").toInt(), 1);
    QCOMPARE(_::Private::read(value, "value1").toInt(), 1);
}

void QuickTests::spec_QJSValue_is_object()
{
    QJSEngine engine;

    QJSValue object = engine.newObject();

    _::Private::write(object, "value1", 1);

    QCOMPARE(static_cast<bool>(_::Private::key_value_support_path_object_creation<QJSValue>::value), true);
    QCOMPARE(_::Private::key_value_create_path_object(object).isObject(), true);

    QCOMPARE(_::Private::p_isForInAble_(object), true);
    QCOMPARE(object.property("value1").toInt(), 1);
    QCOMPARE(_::Private::read(object, "value1").toInt(), 1);
}

void QuickTests::spec_QJSValue_is_array() {
    QJSEngine engine;
    QJSValue object = engine.newArray();

    QCOMPARE(_::isCollection(object), true);

}

void QuickTests::test_private_is_convertible_args_QJSValue_QVariant()
{
    QCOMPARE( static_cast<bool> (_::Private::is_custom_convertible<QJSValue, QVariant>::value) , true);

    QQmlApplicationEngine engine;
    QString content = QtShell::cat(QString(SRCDIR) + "/SampleData1.json");
    QJSValue source = engine.evaluate(content);

    QVariant dest;
    _::Private::convertTo(source, dest);

    auto map = dest.toMap();

    QCOMPARE(map["value1"].toInt(), 10);
}

void QuickTests::test_forIn_arg1_QJSValue()
{
    {
        QQmlApplicationEngine engine;

        QString content = QtShell::cat(QString(SRCDIR) + "/SampleData1.json");
        QJSValue value = engine.evaluate(content);

        QVariantMap map;

        _::forIn(value, [&](const QJSValue& value, const QString& key) {
            map[key] = value.toVariant();
        });

        QCOMPARE(map["value1"].toInt()  , 10);
        QCOMPARE(map["value3"].toBool() , false);
        QCOMPARE(map["value4"].toMap()["value1"].toInt() , 21);
    }

}

void QuickTests::test_assign_arg1_QJSValue_arg2_other()
{
    {
        QQmlApplicationEngine engine;
        engine.load(QString(SRCDIR) + "/SampleData1.qml");
        QObject* root = engine.rootObjects()[0];

        QString content = QtShell::cat(QString(SRCDIR) + "/SampleData1.json");
        QJSValue value = engine.evaluate(content);

        QCOMPARE(value.property("value1").toInt(), 10);
        QCOMPARE(root->property("value1").toInt(), 1);

        _::assign(root, value);

        QCOMPARE(root->property("value1").toInt(), 10);
        QVERIFY(root->property("value2").toString() == "11");
        QVERIFY(root->property("value3").toBool() == false);
        QVERIFY(root->property("value4").canConvert<QJSValue>());
    }

    {
        QQmlApplicationEngine engine;
        QString content = QtShell::cat(QString(SRCDIR) + "/SampleData1.json");
        QJSValue value = engine.evaluate(content);

        QObject* object = nullptr;
        _::assign(object, value);
    }
}

void QuickTests::test_merge_arg1_QJSValue_arg2_other()
{
    /* Remarks: merge(QJSValue, any_type_other_then_QJSValue) is not supported.
     */

    {
        /* QVariantMap, QJSValue */
        QQmlApplicationEngine engine;

        QString content;
        content = QtShell::cat(QString(SRCDIR) + "/SampleData1.json");
        qDebug().noquote() << content;
        QJSValue source = engine.evaluate(content);

        QVariantMap object;
        object["value4"] = QVariantMap{{"value2", 2.0}};

        qDebug().noquote() << object;

        _::merge(object, source);

        qDebug().noquote() << object;

        QCOMPARE(object["value4"].toMap()["value1"].toInt(), 21);
        QCOMPARE(object["value4"].toMap()["value2"].toDouble(), 2.0);
    }

    {
        /* QJSValue, QJSValue */
        QQmlApplicationEngine engine;

        QString content;
        content = QtShell::cat(QString(SRCDIR) + "/SampleData1.json");
        qDebug() << content;
        QJSValue source = engine.evaluate(content);
        QCOMPARE(source.property("value4").property("value1").toInt(), 21);

        content = QtShell::cat(QString(SRCDIR) + "/SampleData2.json");
        qDebug() << content;
        QJSValue object = engine.evaluate(content);
        QCOMPARE(object.property("value4").property("value2").toNumber(), 2.0);

        object.property("value4").setProperty("value1", 10);
        QCOMPARE(object.property("value4").property("value1").toInt(), 10);

        qDebug() << "";
        qDebug() << "Condition: _::merge(QJSValue, QJSValue)";
        _::merge(object, source);

        QCOMPARE(object.property("value4").property("value1").toInt(), 21);
        QCOMPARE(object.property("value4").property("value2").toNumber(), 2.0);

        qDebug() << "";
        qDebug() << "Condition: _::merge(QVariant, QJSValue)";
        QVariantMap map;
        _::merge(map, object);

        QCOMPARE(map["value4"].toMap()["value1"].toInt(), 21);
        QCOMPARE(map["value4"].toMap()["value2"].toDouble(), 2.0);
    }

}

void QuickTests::spec_merge_args_QJSValue_QJSValue_should_support_missing_path_creation()
{
    QQmlEngine engine;
    QJSValue object = engine.newObject();

    QJSValue source = engine.toScriptValue(QVariantMap{{ "a", QVariantMap {{"b", 2.0}}}});

    _::merge(object, source);

    QCOMPARE(object.property("a").isUndefined(), false);
    QCOMPARE(object.property("a").property("b").toNumber(), 2.0);
}

void QuickTests::test_set_args_QJSValue_key_QJSValue()
{
    QVariantMap templ;
    _::set(templ, "value1.value1", 1);
    _::set(templ, "value1.value2.value2", 2.0);

    QQmlEngine engine;
    QJSValue object = engine.toScriptValue(templ);
    QJSValue value = engine.toScriptValue(QString("3"));

    _::set(object, "value1.value2.value1", value);

    QCOMPARE(object.property("value1").property("value2").property("value1").toString(), QString("3"));
}

void QuickTests::spec_set_args_QJSValue_key_QJSValue_should_support_missing_path_creation()
{
    QVariantMap templ;
    _::set(templ, "value1.value1", 1);
    _::set(templ, "value1.value2.value2", 2.0);

    QQmlEngine engine;
    QJSValue object = engine.toScriptValue(templ);
    QJSValue value = engine.toScriptValue(QString("3"));

    _::set(object, "value1.value3.value1", value);

    QCOMPARE(object.property("value1").property("value3").property("value1").toString(), QString("3"));
}

void QuickTests::test_set_args_QVariantMap_key_QJSValue()
{
    QVariantMap object;
    _::set(object, "value1.value1", 1);
    _::set(object, "value1.value2.value2", 2.0);
    QQmlEngine engine;
    QJSValue value = engine.toScriptValue(QString("3"));

    _::set(object, "value1.value2.value1", value);

    QVariant actualValue = _::get(object, "value1.value2.value1");

    QCOMPARE(actualValue.toString(), QString("3"));
}

void QuickTests::test_omit_arg1_QJSValue()
{
    QQmlApplicationEngine engine;

    QString content = QtShell::cat(QString(SRCDIR) + "/SampleData1.json");
    QJSValue object = engine.evaluate(content);

    QVariantMap result = _::omit(object, QStringList{"value2", "value4.value1"});

    QCOMPARE(result["value1"].toInt(), 10);
    QCOMPARE(result.contains("value2"), false);
    QCOMPARE(result.contains("value4"), true);

    QCOMPARE(_::get(result, "value4.value1").isNull(), true);
}
