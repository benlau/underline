#pragma once
#include <QObject>

class TestCases : public QObject
{
    Q_OBJECT
public:
    explicit TestCases(QObject *parent = 0);

private slots:

    void test_some();

    void test_map();
};

