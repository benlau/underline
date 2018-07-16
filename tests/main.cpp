#include <QString>
#include <QtTest>
#include <TestRunner>
#include <QtQuickTest/quicktest.h>
#include <XBacktrace.h>
#include <QtShell>
#include "testcases.h"
#include "underline.h"

int main(int argc, char *argv[])
{
    XBacktrace::enableBacktraceLogOnUnhandledException();

    QGuiApplication app(argc, argv);

    TestRunner runner;
    runner.add<TestCases>();

    bool error = runner.exec(app.arguments());

    if (!error) {
        qDebug() << "All test cases passed!";
    }

    return error;
}
