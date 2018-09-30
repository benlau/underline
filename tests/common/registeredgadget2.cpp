#include <QCoreApplication>
#include "registeredgadget.h"
#include "underline.h"
#include "registeredgadget2.h"

RegisteredGadget2::RegisteredGadget2()
{

}

QList<RegisteredGadget> RegisteredGadget2::list() const
{
    return m_list;
}

void RegisteredGadget2::setList(const QList<RegisteredGadget> &list)
{
    m_list = list;
}

static void init() {
    _::registerQtMetable<RegisteredGadget2>();
}

Q_COREAPP_STARTUP_FUNCTION(init);
