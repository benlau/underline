#pragma once
#include <QMap>

template <typename T>
class Storage {
public:

    static QMap<int,T>& getContent() {
        static QMap<int, T> content;
        return content;
    }

};
