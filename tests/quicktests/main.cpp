#include <QString>
#include <QtTest>
#include <TestRunner>
#include <QtQuickTest/quicktest.h>
#include <XBacktrace.h>
#include <QtShell>
#include "quicktests.h"
#include "underline.h"

int main(int argc, char *argv[])
{
    XBacktrace::enableBacktraceLogOnUnhandledException();
    qputenv("QML_DISABLE_DISK_CACHE", "true");

    QCoreApplication app(argc, argv);

    TestRunner runner;
    runner.add<QuickTests>();

    bool error = runner.exec(app.arguments());

    if (!error) {
        qDebug() << "All test cases passed!";
    }

    return error;
}
