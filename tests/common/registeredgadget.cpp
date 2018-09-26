#include <QCoreApplication>
#include "registeredgadget.h"
#include "underline.h"

RegisteredGadget::RegisteredGadget()
{
    value = 0;
}

static void init() {
    _::registerQtMetable<RegisteredGadget>();
}

Q_COREAPP_STARTUP_FUNCTION(init);
