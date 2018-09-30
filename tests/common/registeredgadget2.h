#ifndef REGISTEREDGADGET2_H
#define REGISTEREDGADGET2_H

#include "registeredgadget.h"

class RegisteredGadget2
{
    Q_GADGET
    Q_PROPERTY(QList<RegisteredGadget> list READ list WRITE setList)
public:
    RegisteredGadget2();

    QList<RegisteredGadget> list() const;
    void setList(const QList<RegisteredGadget> &list);

private:
    QList<RegisteredGadget> m_list;

};

#endif // REGISTEREDGADGET2_H
