#ifndef UTILS_H
#define UTILS_H

#include <QMap>

class Utils
{
public:
    Utils();

    static QMap<int,int>& getContentFromStorage();

};

#endif // UTILS_H
