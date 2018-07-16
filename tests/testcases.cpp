#include <QQmlApplicationEngine>
#include <QTest>
#include <Automator>
#include <QtShell>
#include <functional>
#include "testcases.h"
#include "underline.h"

static bool isOdd(int value) {
    return value % 2 == 1;
}

TestCases::TestCases(QObject *parent) : QObject(parent)
{
    auto ref = [=]() {
        QTest::qExec(this, 0, 0); // Autotest detect available test cases of a QObject by looking for "QTest::qExec" in source code
    };
    Q_UNUSED(ref);
}

void TestCases::test_function()
{
    auto func = [](auto i) {
        Q_UNUSED(i);
    };

    QCOMPARE((std::is_convertible<std::function<void(int)>, decltype(func)>::value), false);

    QCOMPARE((std::is_convertible<decltype(func), std::function<void(int)>>::value), true);
}

void TestCases::test_some()
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
            Q_UNUSED(index);
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
        /// test non-int function
        auto func = [](QString item) {
            return item.size() % 2 == 1;
        };

        QCOMPARE(_::some(QStringList() << "0" << "2" << "4", func), true);
        QCOMPARE(_::some(QStringList() << "01" << "21" << "42", func), false);

    }

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

