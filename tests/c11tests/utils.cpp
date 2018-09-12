#include "utils.h"
#include "storage.h"

Utils::Utils()
{

}

QMap<int, int>& Utils::getContentFromStorage()
{
    return Storage<int>::getContent();
}
