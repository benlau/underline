#ifndef GADGETOBJECT_H
#define GADGETOBJECT_H

#include <QObject>

class GadgetObject {
    Q_GADGET

    Q_PROPERTY(int value MEMBER value)
public:

    int value;
};

#endif // GADGETOBJECT_H
