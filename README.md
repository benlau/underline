## Underline
[![Build Status](https://www.travis-ci.org/benlau/underline.svg?branch=master)](https://www.travis-ci.org/benlau/underline)
[![Build status](https://ci.appveyor.com/api/projects/status/p6jsldfoj73ep630?svg=true)](https://ci.appveyor.com/project/benlau/underline)

A Qt C++ utility library provides useful functional programming helpers like lodash.js

Features:
-----

1. It is a C++ 11 library but supports C++14's generic lambda function (using auto as parameter type)

2. Support Qt types but it is still compilable even the Qt library is missing.

3. Eliminate misleading template error messages by using static_assert. Easier to debug template error.

4. Single Header Library

5. All the helper functions are pure, reentrant, and thead-safe

Use-cases
-------

 1) Serialize a QObject
```C++
#include <underline.h>

  // Non-deep copy from source to dest
  _::assign( /* QVariantMap */ dest, /* QObject* */ source );

  // Serialize but ignore the parent object
  dest = _::omit(source, QStringList{"parent"});
```

2) Avoid using method chaining to ask for a property value.

```C++
// Obtain the objectName property from object's parent.
// It is equivalent to object->parent()->objectName() but _::get will take care null pointer checking
QVariant property = _::get(object, "parent.objectName");
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

```C++
_:assign(QVariantMap& dest, QObject source, ...)
_:assign(QObject* dest, QObject* source, ...)
_:assign(QVariantMap& dest, QVariantMap source, ...)
_:assign(QObject* dest, QJSValue source, ...) // Available only if `quick` is set in the qmake project file.
```

Assigns the string-keyed properties from the source object to the destination object. The source objects are applied from left to right. Subsequent sources overwrite property assignments of previous sources.

Arguments:

 * dest: The destination object
 * source && ... : The source object(s) to be assigned to the destination object

Returns:

 * None

Example

```C++
_::assign(object, QVariantMap{{"objectName", "Test"}});
_::assign(map, object, QVariantMap{{"objectName", "Test"}});
```

TODO

1. Returns the destination object

get
---

```C++
QVariant _::get(const QObject* source, QString path,QVariant defaultValue = QVariant())
QVariant _::get(const QVariantMap source, QString path, QVariant defaultValue = QVariant())
```

Obtain the value from the source object at the given path. If the path does not exist, it will return the default value.

Example:

```
// Obtain the objectName property from object's parent
QVariant property = _::get(object, "parent.objectName");
```

clamp
-----



map
----

```C++
_::map(collection, iteratee)
```

Creates an array of values by running each element in collection throught iteratee. The iteratee is invoked with three arguments: (value, index, collection). The index and collection parameters are optional.


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

```
QVariantMap omit(QVariantMap source, QVariantMap paths)
```

Creates a new QVariantMap object which is a clone of the source, but the properties listed in the paths are omitted.

Arguments

 * source: The source object
 * paths: The property paths to omit

Returns

 * QVariantMap: Returns the new object.

TODO:
```
QVariantMap omit(QVariantMap source, QStringList paths)
```


pick
---

set
---

some
----
