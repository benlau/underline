#ifndef REGISTEREDGADGET_H
#define REGISTEREDGADGET_H

#include <QObject>

class RegisteredGadget
{   
    Q_GADGET

    Q_PROPERTY(int value MEMBER value)
public:
    RegisteredGadget(int value = 0);

    int value;
};

#endif // REGISTEREDGADGET_H
