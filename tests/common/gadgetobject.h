#ifndef GADGETOBJECT_H
#define GADGETOBJECT_H

#include <QObject>

class GadgetObject {
    Q_GADGET

    Q_PROPERTY(int value MEMBER value)
public:

    int value;
};

Q_DECLARE_METATYPE(GadgetObject)

#endif // GADGETOBJECT_H
