#include <QString>
#include <QtTest>
#include <QApplication>
#include "quicktests.h"
#include "underline.h"

int main(int argc, char *argv[])
{
    qputenv("QML_DISABLE_DISK_CACHE", "true");

    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    QuickTests tc;
    return QTest::qExec(&tc, argc, argv);
}

