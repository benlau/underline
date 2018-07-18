#include <QString>
#include <QtTest>
#include <TestRunner>
#include <QtQuickTest/quicktest.h>
#include <XBacktrace.h>
#include <QtShell>
#include "c11testcases.h"
#include "underline.h"

int main(int argc, char *argv[])
{
    XBacktrace::enableBacktraceLogOnUnhandledException();

    QCoreApplication app(argc, argv);

    TestRunner runner;
    runner.add<C11TestCases>();

    bool error = runner.exec(app.arguments());

    if (!error) {
        qDebug() << "All test cases passed!";
    }

    return error;
}
