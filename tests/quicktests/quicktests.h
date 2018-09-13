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
    void validate_QJSValue_expected_behaviour();

    void spec_QJSValue();

    void spec_QJSValue_is_object();

    void spec_QJSValue_is_array();

    void test_private_is_convertible_args_QJSValue_QVariant();

    void test_forIn_arg1_QJSValue();

    void test_forEach_arg1_QJSValue();

    void test_assign_arg1_QJSValue_arg2_other();

    void test_merge_arg1_QJSValue_arg2_other();

    void spec_merge_args_QJSValue_QJSValue_should_support_missing_path_creation();

    void spec_merge_args_QJSValue_QJSValue_should_support_list_merging();

    void test_set_args_QJSValue_key_QJSValue();

    void spec_set_args_QJSValue_key_QJSValue_should_support_missing_path_creation();

    void test_set_args_QVariantMap_key_QJSValue();

    void test_omit_arg1_QJSValue();

};

#endif // QUICKTESTS_H
