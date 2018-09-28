#include <QCoreApplication>
#include "registeredgadget.h"
#include "underline.h"

RegisteredGadget::RegisteredGadget(int value) : value(value)
{
}

static void init() {
    _::registerQtMetable<RegisteredGadget>();
}

Q_COREAPP_STARTUP_FUNCTION(init);
