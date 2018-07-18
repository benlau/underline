#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include <QObject>
#include "dataobject2.h"

class DataObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int value1 READ value1 WRITE setValue1 NOTIFY value1Changed)
    Q_PROPERTY(double value2 READ value2 WRITE setValue2 NOTIFY value2Changed)
    Q_PROPERTY(QString value3 READ value3 WRITE setValue3 NOTIFY value3Changed)
    Q_PROPERTY(DataObject2* value4 READ value4 WRITE setValue4 NOTIFY value4Changed)

public:
    explicit DataObject(QObject *parent = nullptr);

    int value1() const;
    void setValue1(int value1);

    double value2() const;
    void setValue2(double value2);

    QString value3() const;
    void setValue3(const QString &value3);

    DataObject2 *value4() const;
    void setValue4(DataObject2 *value4);

signals:
    void value1Changed();
    void value2Changed();
    void value3Changed();
    void value4Changed();

public slots:

private:
    int m_value1;
    double m_value2;
    QString m_value3;
    DataObject2* m_value4;

};

#endif // DATAOBJECT_H
