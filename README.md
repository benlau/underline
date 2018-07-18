It is still under development

Features:

1. It is a C++ 11 library but supports C++14's generic lambda function
2. Single header library

Example:

```C++

#include <underline.h>

QList<int> output1 = _::map(QList<QString>() << "1" << "2" << "3", [](auto item) { return item.toInt();});

QVector<int> output2 = _::map(QVector<QString>() << "1" << "2" << "3", [](auto item) { return item.toInt();});

QVector<int> output3 = _::map(QVector<QString>() << "1" << "2" << "3", [](auto item, int index) { return item.toInt();});

```
