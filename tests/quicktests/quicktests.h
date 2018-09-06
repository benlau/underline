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

    void test_forIn_arg1_QJSValue();

    void test_assign_arg1_QJSValue_arg2_other();

    void test_merge_arg1_QJSValue_arg2_other();

    void test_set_args_QJSValue_key_QJSValue();

    void test_set_args_QJSValue_key_QJSValue_shouldnt_create_non_exist_path();

    void test_set_args_QVariantMap_key_QJSValue();

    void test_omit_arg1_QJSValue();

};

#endif // QUICKTESTS_H
