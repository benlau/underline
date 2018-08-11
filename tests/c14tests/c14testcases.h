#pragma once
#include <QObject>

class C14TestCases : public QObject
{
    Q_OBJECT
public:
    explicit C14TestCases(QObject *parent = 0);

private slots:

    void test_private_is_qobject();

    void test_private_traits();

    void test_private_invoke();

    void test_private_value();

    void test_some();

    void test_map();

    void test_assign();

    void test_set();

    void test_get();

    void test_pick();

    void test_omit();

    void test_forIn();

    void test_forEach();

    void test_reduce();

    void test_countBy();

};

