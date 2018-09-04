#include "dataobject.h"

DataObject::DataObject(QObject *parent) : QObject(parent)
{
    setObjectName("Root");
    m_value4 = new DataObject2(this);
    m_value1 = 1;
    m_value2 = 2.0;
    m_value3 = "3";
}

int DataObject::value1() const
{
    return m_value1;
}

void DataObject::setValue1(int value1)
{
    m_value1 = value1;
}

double DataObject::value2() const
{
    return m_value2;
}

void DataObject::setValue2(double value2)
{
    m_value2 = value2;
}

QString DataObject::value3() const
{
    return m_value3;
}

void DataObject::setValue3(const QString &value3)
{
    m_value3 = value3;
}

DataObject2 *DataObject::value4() const
{
    return m_value4;
}

void DataObject::setValue4(DataObject2 *value4)
{
    m_value4 = value4;
}
