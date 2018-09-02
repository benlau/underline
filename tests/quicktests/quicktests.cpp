#include <QTest>
#include <QQmlApplicationEngine>
#include <QtShell>
#include "quicktests.h"

#define __UNDERLINE_DEBUG(x) { qDebug() << x;}
#include <underline.h>

QuickTests::QuickTests(QObject *parent) : QObject(parent)
{
    auto ref = [=]() {
        QTest::qExec(this, 0, 0); // Autotest detect available test cases of a QObject by looking for "QTest::qExec" in source code
    };
    Q_UNUSED(ref);
}

void QuickTests::test_QJSValue()
{
    QCOMPARE((bool) _::Private::is_meta_object<QJSValue>::value, true);

    QJSEngine engine;
    QJSValue value = engine.toScriptValue(QVariantMap{});

    _::Private::write(value, "value1", 1);

    QCOMPARE(value.property("value1").toInt(), 1);

    QCOMPARE(_::Private::read(value, "value1").toInt(), 1);
}

void QuickTests::test_private_is_convertible()
{
    QCOMPARE( (bool) (_::Private::is_custom_convertible<QJSValue, QVariant>::value) , true);

    QQmlApplicationEngine engine;
    QString content = QtShell::cat(QString(SRCDIR) + "/SampleData1.json");
    QJSValue source = engine.evaluate(content);

    QVariant dest;
    _::Private::convertTo(source, dest);

    auto map = dest.toMap();

    QCOMPARE(map["value1"].toInt(), 10);
}

void QuickTests::test_forIn()
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

void QuickTests::test_assign_QJSValue()
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

void QuickTests::test_merge_QJSValue()
{
    /* Remarks: merge(QJSValue, any_type_other_then_QJSValue) is not supported.
     */

    {
        /* QVariantMap, QJSValue */
        QQmlApplicationEngine engine;

        QString content;
        content = QtShell::cat(QString(SRCDIR) + "/SampleData1.json");
        qDebug() << content;
        QJSValue source = engine.evaluate(content);

        QVariantMap object;
        object["value4"] = QVariantMap{{"value2", 2.0}};

        _::merge(object, source);

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

        //@TODO
        _::merge(object, source);
        _::merge(object, source);

        QCOMPARE(object.property("value4").property("value1").toInt(), 21);
        QCOMPARE(object.property("value4").property("value2").toNumber(), 2.0);

        QVariantMap map;
        _::merge(map, object);

        QCOMPARE(map["value4"].toMap()["value1"].toInt(), 21);
        QCOMPARE(map["value4"].toMap()["value2"].toDouble(), 2.0);

    }

}
