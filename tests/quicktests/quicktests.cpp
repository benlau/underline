#include <QTest>
#include <QQmlApplicationEngine>
#include <QtShell>
#include <underline.h>
#include "quicktests.h"

QuickTests::QuickTests(QObject *parent) : QObject(parent)
{
    auto ref = [=]() {
        QTest::qExec(this, 0, 0); // Autotest detect available test cases of a QObject by looking for "QTest::qExec" in source code
    };
    Q_UNUSED(ref);

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

        _::assign(root, value);

        QCOMPARE(root->property("value1").toInt(), 10);
        QVERIFY(root->property("value2").toString() == "11");
        QVERIFY(root->property("value3").toBool() == false);
        QCOMPARE(root->property("value4").value<QObject*>()->property("value1").toInt(), 21);
    }

    {
        QQmlApplicationEngine engine;
        QString content = QtShell::cat(QString(SRCDIR) + "/SampleData1.json");
        QJSValue value = engine.evaluate(content);

        _::assign(0, value);
    }
}
