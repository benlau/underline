#include <QQmlApplicationEngine>
#include <QTest>
#include <Automator>
#include <QtShell>
#include <functional>
#include "testcases.h"
#include "underline.h"

TestCases::TestCases(QObject *parent) : QObject(parent)
{
    auto ref = [=]() {
        QTest::qExec(this, 0, 0); // Autotest detect available test cases of a QObject by looking for "QTest::qExec" in source code
    };
    Q_UNUSED(ref);
}

void TestCases::test_some()
{
    auto isOdd = [](int item, int index) {
        Q_UNUSED(index);
        return item % 2 == 1;
    };

    QCOMPARE(_::some(QList<int>() << 0 << 2 << 4, isOdd), false);
    QCOMPARE(_::some(QList<int>() << 0 << 3 << 4, isOdd), true);
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

