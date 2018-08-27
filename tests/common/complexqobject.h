#ifndef COMPLEXQOBJECT_H
#define COMPLEXQOBJECT_H

#include <QObject>
#include <QVariantMap>
#include <dataobject.h>
#include <gadgetobject.h>

class ComplexQObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(GadgetObject value1 READ value1 WRITE setValue1 NOTIFY value1Changed)
    Q_PROPERTY(DataObject* value2 READ value2)
    Q_PROPERTY(QVariantMap value3 READ value3 WRITE setValue3 NOTIFY value3Changed)
    Q_PROPERTY(QStringList value4 READ value4 WRITE setValue4 NOTIFY value4Changed)

public:
    explicit ComplexQObject(QObject *parent = nullptr);

    GadgetObject value1() const;
    void setValue1(const GadgetObject &value1);

    DataObject *value2() const;

    QVariantMap value3() const;
    void setValue3(const QVariantMap &value3);

    QStringList value4() const;
    void setValue4(const QStringList &value4);

signals:
    void value1Changed();
    void value3Changed();
    void value4Changed();

public slots:

private:
    GadgetObject m_value1;
    DataObject* m_value2;
    QVariantMap m_value3;
    QStringList m_value4;

};

#endif // COMPLEXQOBJECT_H
