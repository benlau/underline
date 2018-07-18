#pragma once
#include <QObject>

class C14TestCases : public QObject
{
    Q_OBJECT
public:
    explicit C14TestCases(QObject *parent = 0);

private slots:

    void test_private_traits();

    void test_private_invoke();

    void test_some();

    void test_map();
};

