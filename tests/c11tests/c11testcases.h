#pragma once
#include <QObject>

class C11TestCases : public QObject
{
    Q_OBJECT
public:
    explicit C11TestCases(QObject *parent = 0);

private slots:

    void test_private_traits();

    void test_private_invoke();

    void test_some();

    void test_map();
};

