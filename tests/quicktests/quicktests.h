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
    void test_spec_QJSValue();

    void test_private_is_convertible_args_QJSValue_QVariant();

    void test_forIn();

    void test_assign_args_QJSValue_other();

    void test_merge_QJSValue_other();
};

#endif // QUICKTESTS_H
