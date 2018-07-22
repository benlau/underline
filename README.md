## Underline
[![Build Status](https://www.travis-ci.org/benlau/underline.svg?branch=master)](https://www.travis-ci.org/benlau/underline)
[![Build status](https://ci.appveyor.com/api/projects/status/p6jsldfoj73ep630?svg=true)](https://ci.appveyor.com/project/benlau/underline)

A C++ utility library provides useful functional programming helpers like lodash.js

Features:
-----

**1) C++11 compliant coding. Support C++14 generic lambda function (using auto as parameter) and return type detection.**

```C++

std::vector<int> output1 = _::map(std::vector<std::string>{"1","2","3"},
                              [](auto item) { return std::stoi(item);});

QList<int> output2 = _::map(QList<QString>{"1","2","3"},
                            [](auto item, auto index) {  return item.toInt() + index;});

QVector<int> output3 = _::map(QVector<QString>{"1","2","3"},
                              [](QString, int index, auto collection) { return collection[index].toInt();});
```

**2) Support Qt types but it is still compilable even the Qt library is missing.**

```C++
// Serialize a QObject
// Non-deep copy from source to dest
_::assign( /* QVariantMap */ dest, /* QObject* */ source );

// Obtain the objectName property from object's parent.
// It is equivalent to object->parent()->objectName() but _::get will take care null pointer checking
QVariant property = _::get(object, "parent.objectName");
```

**3) Eliminate misleading template error messages for mismatched argument type. Easier to debug template error.**

Example:

```C++
    auto output = _::map(QList<QString>{"1","2","3"}, [](auto item, QString index) { return item;});
```

The 2nd argument of the iteratee function should be an int type but QString is given. Normally, it will trigger a compilation error at somewhere that user has no idea what is it actually doing. The error message is super long and difficult to understand.

Underline captures the argument type mismatched error by using static_assert(). The actual error message on above example will be

```
error: static_assert failed "_::map(): Mismatched argument types in the iteratee function. Please validate the number of argument and their type."
```

**4) Single Header Library**

**5) All the helper functions are pure, reentrant, and thead-safe**

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

Installation
-----------

1) Method 1: Download from Web

```
wget https://raw.githubusercontent.com/benlau/underline/master/src/cpp/underline.h
```

2) Method 2: Git Subtree

```
git subtree add -P lib/underline --squash https://github.com/benlau/underline.git src-subtree
```

Remarks: This method may not get the latest version.

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

 * The dest object

Example

```C++
_::assign(object, QVariantMap{{"objectName", "Test"}});
_::assign(map, object, QVariantMap{{"objectName", "Test"}});
```

clamp
-----

```
_::clamp(number, lower, upper)
```

Clamps number within the inclusive lower and upper bounds.

Arguments:
 * number: The number to clamp.
 * lower: The lower bound.
 * upper: The upper bound.

Returns

 * The clamped number.

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


forIn
-----

```
_::forIn(Map object, iteratee) // std::map, QMap, QVariantMap
_::forIn(QObject* object, iteratee)
```

Iterates all the string keyed proeprties of an object and calls iteratee function. The iteratee is invoked with one to three arguments: [value, [key, [collection]]].

Arguments:

 * object: The source of object. Support types: QVariantMap
 * iteratee: The function invoked per iteration.

Returns:

 * object

Example
```
```

map
----

```C++
_::map(collection, iteratee)
```

Creates an array of values by running each element in collection throught iteratee. The iteratee is invoked with one to three arguments: [value, [index, [collection]]].

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

Creates a new object which is a clone of the source, but the properties listed in the paths are omitted.

Arguments

 * source: The source object
 * paths: The property paths to omit

Returns

 * QVariantMap: Returns the new object.

TODO:

 * QVariantMap omit(QVariantMap source, QStringList paths)
 * omit(Map source, QStringList paths)
 * omit(QObject* source, QStringList paths)

pick
---

```
QVariantMap _::pick(QObject* source, QStringList &paths);
QVariantMap _::pick(QVariantMap source, QStringList &paths);

```

Creates a new object which is a clone of the source but only the properties listed in the paths are picked.

Arguments

 * source: The source object
 * paths: The property paths to pick

Returns

 * QVariantMap: Returns the new object.


set
---


```C++
void _::set(QVariantMap &object, const QStringList &path, const QVariant &value)
void _::set(QVariantMap &object, const QString &path, const QVariant &value)
```

Set the property from the source object at the given path. If the path does not exist, it will do nothinbg.

Arguments

 * source: The input object
 * paths: The property paths to set
 * value: The property value to be set

Returns

 * void


Example:

some
----

```c++
bool _::some(Collection collection, Predicate predicate)
```

Check if predicate function returns a truth value for any element in the collection. The iteration is stopped once it got a truth value. The process will terminate once it is a truth value. The predicate function is invoked with one to three arguments: [value, [index, [collection]]].

Arguments

 * collection: The input collection
 * predicate: The predicate function to be invoked per iteration

Returns

 * bool: Returns true if any element passes to the predicate function, otherwise, it is false.

