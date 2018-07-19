It is still under development
## Underline

A C++ utility library provides useful functional programming helpers like lodash.js

Features:
-----
1. It is a C++ 11 library but supports C++14's generic lambda function
2. Single header library

Example
-------

```C++

#include <underline.h>

QList<int> output1 = _::map(QList<QString>() << "1" << "2" << "3", [](auto item) { return item.toInt();});

QVector<int> output2 = _::map(QVector<QString>() << "1" << "2" << "3", [](auto item) { return item.toInt();});

QVector<int> output3 = _::map(QVector<QString>() << "1" << "2" << "3", [](auto item, int index) { return item.toInt();});

```

API
===

assign
-----

Assigns the string-keyed properties from the source object to the destination object. The source objects are applied from left to right. Subsequent sources overwrite property assignments of previous sources.

Arguments

```C++
_:assign(QVariantMap& object, QObject source, ...)
_:assign(QObject* object, QObject* source, ...)
_:assign(QVariantMap& object, QVariantMap object, ...)
```


get
---

map
omit
pick,
set
some
