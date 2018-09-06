#pragma once
#include <QObject>

class C11TestCases : public QObject
{
    Q_OBJECT
public:
    explicit C11TestCases(QObject *parent = 0);

private slots:

    void initTestCase();

    void test_private_has();

    void test_private_is_array();

    void test_private_is_map();

    void test_private_is_meta_object();

    void test_private_is_qobject();

    void test_private_is_key_value_type();

    void test_private_traits();

    void test_private_invoke();

    void test_private_invoke_by_read();

    void test_private_rebind_to_map();

    void test_private_has_static_meta_object();

    void test_private_read();

    void test_private_write();

    void test_private_cast_to_pointer();

    void test_private_cast_to_qobject();

    void test_private_merge();

    void test_private_GadgetContainer();

    void test_private_contains();

    void test_merge();

    void test_merge_qobject();

    void test_merge_gadget();

    void test_merge_complex();

    void test_some();

    void test_map();

    void test_range();

    void test_rangeQ();

    void test_isMap();

    void test_isKeyValueType();

    void test_isQtMetable();

};

