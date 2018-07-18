#ifndef DATAOBJECT2_H
#define DATAOBJECT2_H

#include <QObject>

class DataObject2 : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int value1 READ value1 WRITE setValue1 NOTIFY value1Changed)
public:
    explicit DataObject2(QObject *parent = nullptr);

    int value1() const;
    void setValue1(int value1);

signals:
    void value1Changed();

public slots:

private:
    int m_value1;
};

#endif // DATAOBJECT2_H
