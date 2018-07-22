#include "dataobject2.h"

DataObject2::DataObject2(QObject *parent) : QObject(parent)
{
    m_value1 = 5;
}

int DataObject2::value1() const
{
    return m_value1;
}

void DataObject2::setValue1(int value4)
{
    m_value1 = value4;
}
