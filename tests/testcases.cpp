#include <QQmlApplicationEngine>
#include <QTest>
#include <Automator>
#include <QtShell>
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

