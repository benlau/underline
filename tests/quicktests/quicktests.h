#ifndef QUICKTESTS_H
#define QUICKTESTS_H

#include <QObject>

class QuickTests : public QObject
{
    Q_OBJECT
public:
    explicit QuickTests(QObject *parent = nullptr);

signals:

public slots:

private slots:
    void test_assign_QJSValue();
};

#endif // QUICKTESTS_H
