#pragma once
#include <QObject>

class C11TestCases : public QObject
{
    Q_OBJECT
public:
    explicit C11TestCases(QObject *parent = 0);

private slots:

    void test_is_collection();

    void test_private_traits();

    void test_private_invoke();

    void test_private_rebind_to_map();

    void test_has_static_meta_object();

    void test_private_read();

    void test_some();

    void test_map();

    void test_range();

    void test_range_q();
};

