#include "complexqobject.h"

ComplexQObject::ComplexQObject(QObject *parent) : QObject(parent)
{
    m_value1.value = 11;
    m_value2 = new DataObject(this);
    m_value2->setValue1(21);
    m_value2->setValue2(22.0);
    m_value2->setValue3("23.0");

    m_value3 = QVariantMap{{"value1", 33}, {"value2", 33.3}};

    m_value4 = QStringList{"value41", "value42"};
}

GadgetObject ComplexQObject::value1() const
{
    return m_value1;
}

void ComplexQObject::setValue1(const GadgetObject &gadget)
{
    m_value1 = gadget;
    value1();
}

DataObject *ComplexQObject::value2() const
{
    return m_value2;
}

void ComplexQObject::setValue2(DataObject *value)
{
    m_value2 = value;
    emit value2Changed();
}

QVariantMap ComplexQObject::value3() const
{
    return m_value3;
}

void ComplexQObject::setValue3(const QVariantMap &value3)
{
    m_value3 = value3;
}

QStringList ComplexQObject::value4() const
{
    return m_value4;
}

void ComplexQObject::setValue4(const QStringList &value4)
{
    m_value4 = value4;
}
