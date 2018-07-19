## Underline

A C++ utility library provides useful functional programming helpers like lodash.js

Features:
-----

1. It is a C++ 11 library but supports C++14's generic lambda function (using auto as parameter type)

2. Single Header Library

Use-cases
-------

Serialize a QObject
```C++
#include <underline.h>
  QVariantMap dest;
  QObject* source;

  // Non-deep copy from object to output
  _::assign(output, object);

  // Serialize but ignore the parent field
  output = _::omit(object, QStringList(){"parent"});
```

```C++

QList<int> output1 = _::map(QList<QString>(){"1","2","3"},  [](auto item) { return item.toInt();});

QVector<int> output2 = _::map(QVector<QString>() {"1","2","3"}, [](auto item, int index) { return item.toInt();});

QVector<int> output3 = _::map(std::vector<QString>(){"1","2","3"}, [](auto item, int index, auto collection) { return item.toInt();});

```

Installation
-----------

```
wget https://raw.githubusercontent.com/benlau/underline/master/src/cpp/underline.h
```

API
===

assign
-----

Assigns the string-keyed properties from the source object to the destination object. The source objects are applied from left to right. Subsequent sources overwrite property assignments of previous sources.

Declaration

```C++
_:assign(QVariantMap& object, QObject source, ...)
_:assign(QObject* object, QObject* source, ...)
_:assign(QVariantMap& object, QVariantMap object, ...)
_:assign(QObject* object, QJSValue object, ...)
```

get
---

map
----

Creates an array of values by running each element in collection throught iteratee. The iteratee is invoked with three arguments: (value, index, collection). The index and collection parameters are optional.

```C++
_::map(collection, iteratee)
```

Arguments:

 * collection: The input collection. QList/QVector/std::vector are proven to work.
 * iteratee: The function invoked per iteration


Returns:

 * (Array) The new mapped array. The container type should be same as the input collection.

Example:

```C++
QList<int> output1 = _::map(QList<QString>(){"1","2","3"},  [](auto item) { return item.toInt();});

QVector<int> output2 = _::map(QVector<QString>() {"1","2","3"}, [](auto item, int index) { return item.toInt();});

QVector<int> output3 = _::map(std::vector<QString>(){"1","2","3"}, [](auto item, int index, auto collection) { return item.toInt();});
```

omit
----

pick
---

set
---

some
----
