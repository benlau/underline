#include <QCoreApplication>
#include "registeredgadget.h"
#include "underline.h"

RegisteredGadget::RegisteredGadget(int value) : value(value)
{
}

static void init() {
    _::registerQtType<RegisteredGadget>();
}

Q_COREAPP_STARTUP_FUNCTION(init);
