#include <QString>
#include <QtTest>
#include <TestRunner>
#include <QtQuickTest/quicktest.h>
#include <XBacktrace.h>
#include <QtShell>
#include "testcases.h"
#include "underline.h"

#define USE_LIBRARY(name) \
    do { extern void __3rdparty_qt_static_library_entry_ ## name(); \
    __3rdparty_qt_static_library_entry_ ## name(); } while (0)

int main(int argc, char *argv[])
{
    XBacktrace::enableBacktraceLogOnUnhandledException();

    QGuiApplication app(argc, argv);

    Q_INIT_RESOURCE(underline);

    // Prevent the init() function for QML type registration to be discarded during static linking
    USE_LIBRARY(Underline);

    TestRunner runner;
    runner.add<TestCases>();

    bool error = runner.exec(app.arguments());

    if (!error) {
        qDebug() << "All test cases passed!";
    }

    return error;
}
