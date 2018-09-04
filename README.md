## Underline
[![Build Status](https://www.travis-ci.org/benlau/underline.svg?branch=master)](https://www.travis-ci.org/benlau/underline)
[![Build status](https://ci.appveyor.com/api/projects/status/p6jsldfoj73ep630?svg=true)](https://ci.appveyor.com/project/benlau/underline)

Underline is a C++ utility library provides useful functional programming helpers like the lodash.js.

Features:
-----

**1) Designed for user-friendly error handling. Get rid of the horrific and misleading template compilation error messages.**

Example:

```C++
    auto output = _::map(QList<QString>{"1","2","3"}, [](auto item, QString index) { return item;});
```

The 2nd argument of the iteratee function should be an int type instead of a QString. Usually, it triggers a compilation error at somewhere that user has no idea what is it doing. The error message is super long and hard to understand.

Underline captures the argument type mismatched error by using static_assert() in the early stage. The actual error message on above example is:

```
error: static_assert failed "_::map(): Mismatched argument types in the iteratee function. Please validate the number of argument and their type."
```

**2) C++11 compliant coding. Support C++14 generic lambda function (using auto as parameter) and return type detection.**

```C++

std::vector<int> output1 = _::map(std::vector<std::string>{"1","2","3"},
                              [](auto item) { return std::stoi(item);});

QList<int> output2 = _::map(QList<QString>{"1","2","3"},
                            [](auto item, auto index) {  return item.toInt() + index;});

QVector<int> output3 = _::map(QVector<QString>{"1","2","3"},
                              [](QString, int index, auto collection) { return collection[index].toInt();});
```

**3) Support Qt types but it is still compilable even the Qt library is missing.**

```C++
// Serialize a QObject
_::merge( /* QVariantMap */ dest, /* QObject* */ source );

// Obtain the objectName property from object's parent.
// It is equivalent to object->parent()->objectName() but _::get will take care null pointer checking
QVariant property = _::get(object, "parent.objectName");
```

**4) Single Header Library**

**5) All the helper functions are reentrant, and thread-safe**

Use-cases for regular C++
-------

```C++
TODO
```

Use-cases for Qt
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

```
wget https://raw.githubusercontent.com/benlau/underline/master/src/cpp/underline.h
```


API
===

assign
-----

```C++
template <typename KeyValueType>
KeyValueType& _::assign(KeyValueType& object, const KeyValueType& source, ...);
```

This function assigns the properties from the source object to the destination object. The input should be an valid Key Value Type where [_::isKeyValueType](#iskeyvaluetype) returns true. The sequence to apply source objects is from left to right. Subsequent sources overwrite property assignments of previous sources. It is a non-recursive function. For deep copying and merge, you should use [_::merge](#merge).

Arguments:

 * object: The destination object
 * source && ... : The source object(s) to be assigned to the destination object. The key and value type of source object should be convertible to the destination object key-value system.

Returns:

 * The destination object

Example

```C++
_::assign(/*QObject*/ object, QVariantMap{{"objectName", "Test"}});
_::assign(map, object, QVariantMap{{"objectName", "Test"}});
```

clamp
-----

```
template <typename Number>
Number _::clamp( number, Number lower, Number upper)
```

Clamps number within the inclusive lower and upper bounds.

Arguments:
 * number: The number to clamp.
 * lower: The lower bound.
 * upper: The upper bound.

Returns

 * The clamped number.

Example
```
    ASSERT_EQ(_::clamp(5,4,7), 5);
    ASSERT_EQ(_::clamp(-7,-4,7), -4);
    ASSERT_EQ(_::clamp(1,-20,-10), -10);
```

countBy
-------

```C++
template <typename Array, typename Iteratee>
Map countBy(const Array& collection, Iteratee iteratee)
```

Iteratees all the elements in the array container ([_::isArray](#isarray) returns true)  and pass to the iteratee function, then count the number of the occurrence of the result. The iteratee function takes only one argument. [value]

The return is a Map object in a type either of std::map or QMap. The actual type chosen depends on the array container. If it is a Qt container class, it will be a QMap. Otherwise, it will be a std::map. The key type is the same as the return of the iteratee, and the value type is an integer storing the counting.

Arguments:

 * collection: The input source. Check [_::isArray](#isarray) function for the supported types
 * iteratee:  The iteratee function to transform the element in the array to a key value.

Examples:

STL:

```C++
    class Student {
    public:
        std::string name;
        int age;
    };

    std::vector<Student> students = {{"Ben", 18} , {"Mary", 17}, {"John", 18}};

    std::map<int, int> stats = _::countBy(students, [=](const Student& student) { return student.age;});
    ASSERT_EQ(stats[18], 2);
    ASSERT_EQ(stats[17], 1);
```

Qt

```C++
    QVariantList students = {QVariantMap{{"name","Ben"},   {"age", 18}},
                             QVariantMap{{"name","Mary"},  {"age", 17}},
                             QVariantMap{{"name","John"},  {"age", 18}}};

    QMap<int,int> stats = _::countBy(students, [=](const QVariant& student) { return student.toMap()["age"].toInt();});
    ASSERT_EQ(stats[18], 2);
    ASSERT_EQ(stats[17], 1);
```

forEach
-------

forIn
-----

```
_::forIn(Map<Key,Value> object, iteratee) // std::map, QMap, QVariantMap
_::forIn(QObject* object, iteratee)
```

Iterates all the string key properties of an object and calls the iteratee function. The iteratee function is invoked with one to three arguments: [value, [key, [collection]]].

Arguments:

 * object: The source object. Support types: QObject*, QMap, QVariantMap, std::map
 * iteratee: The function invoked per iteration. If the source object is a QObject, the data type of the key argument is QString.

Returns:

 * object

Example
```
```

get
---

```C++
/* Qt5 */
QVariant _::get(const KeyValueType& object, QString path,QVariant defaultValue = QVariant())
```

Obtain the value from the source object at the given path. If the path does not exist, it returns the default value.

The input could be any valid Key Value Type where [_::iskeyvaluetype](#isKeyValueType) returns true and using a string as the key type.

Example (Qt):

```
// Obtain the objectName property from object's parent
QVariant property = _::get(/* QObject* */ object, "parent.objectName");
```

map
----

```C++
_::map(Array, Iteratee)
```

Iterate all the elements of the collection and call the iteratee function. It creates a new collection obtain the result. The iteratee is invoked with one to three arguments: [value, [index, [collection]]].

Arguments:

 * collection: The input collection. e.g std::vector, std:list, QList, QVector
 * iteratee: The function invoked per iteration.

Returns:

 * (Collection) The new mapped array. The container type is the same as the input collection.

Example:

```C++
QList<int> output1 = _::map(QList<QString>(){"1","2","3"},  [](auto item) { return item.toInt();});

QVector<int> output2 = _::map(QVector<QString>() {"1","2","3"}, [](auto item, int index) { return item.toInt();});

QVector<int> output3 = _::map(std::vector<QString>(){"1","2","3"}, [](auto item, int index, auto collection) { return item.toInt();});
```

merge
------


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

Set the property from the source object at the given path. If the path does not exist, it does nothing.

Arguments

 * source: The input object
 * paths: The property paths to set
 * value: The property value to be set

Returns

 * void


Example:

range
----

reduce
------


some
----

```c++
bool _::some(Collection collection, Predicate predicate)
```

Check if predicate function returns a truth value for any element in the collection. The iteration stops once it got a truth value. The predicate function is invoked with one to three arguments: [value, [index, [collection]]].

Arguments

 * collection: The input collection
 * predicate: The predicate function to be invoked per iteration

Returns

 * bool: Returns true if any element passes to the predicate function, otherwise, it is false.

Type Checker
============

isArray
------

```C++
bool _::isArray(const T&)
bool _::isArray<T>()
```

It is a static type checker to validate is the input type classified as a valid Array class supported by _.   You rarely need to use this function directly.

Example:

```C++
QCOMPARE(_::isArray(std::vector<int>{}),    true);
QCOMPARE(_::isArray(QVector<int>{ }),       true);
QCOMPARE(_::isArray(QList<int>{ }),         true);
QCOMPARE(_::isArray(QVariantList{ }),       true);
QCOMPARE(_::isArray(QString{ }),            true);

QCOMPARE(_::isArray(std::map<bool,int>{}),  false);
QCOMPARE(_::isArray(QMap<int,int>{}),       false);
QCOMPARE(_::isArray(10),                    false);
```

isKeyValueType
------------

It is a static type checker to validate is the input type classified as a valid Key-Value type supported by  _.  You rarely need to use this function directly.

It is a kind of data structure contains key-value pairs with unique keys. std::map and QMap are the typical examples. In this library, Qt's data types like QObject*, QJSValue and Gadget object also classified as this kind of type. You may use them as an input to functions like [forIn](#forIn), [assign](#assign), and [merge](#merge) etc.

```
ASSERT_EQ(_::isKeyValueType(std::map<int,int>{}),   true);

ASSERT_EQ(_::isKeyValueType(10),                    false);
ASSERT_EQ(_::isKeyValueType(std::vector<int>{}),    false);
```

```
QCOMPARE(_::isKeyValueType(QMap<int,int>{}),       true);
QCOMPARE(_::isKeyValueType(QVariantMap{}),         true);
QCOMPARE(_::isKeyValueType(new QObject(this)),     true);
QCOMPARE(_::isKeyValueType(QJSValue()),            true);

QCOMPARE(_::isKeyValueType(QVariantList{ }),       false);
QCOMPARE(_::isKeyValueType(QString{ }),            false);
```

isQtMetable
------------

```C++
    template <typename T>
    bool isQtMetable();

    template <typename T>
    bool isQtMetable(const T&);
```

It is a static type checker to validate is the input type classified as Qt type with a metable interface supported by _. This kind of object supports a string-keyed properties system. A typical example is QObject* or arbitrary classes with Q_GADGET macro.  QVariantMap and QJSValue are also classified as this kind of type.  You may use them as an input to functions like [forIn](#forIn), [assign](#assign), and [merge](#merge) etc.

You rarely need to use this function directly.

Examples:

```C++
class GadgetObject {
    Q_GADGET
    Q_PROPERTY(int value MEMBER value)
public:
    int value;
};

QCOMPARE(_::isQtMetable(GadgetObject()),        true);
QCOMPARE(_::isQtMetable(QVariantMap{}),         true);
QCOMPARE(_::isQtMetable(new QObject(this)),     true);
QCOMPARE(_::isQtMetable(QJSValue()),            true);
```

isMap
-----

```C++
bool _::isMap(const T&)
bool _::isMap<T>()
```

It is a static type checker to validate is the input type classified as a valid Map container class for _.  You rarely need to use this function directly.

Standard C++

```C++
ASSERT_EQ(_::isMap(std::map<int,int>{}),   true);

ASSERT_EQ(_::isMap(10),                    false);
ASSERT_EQ(_::isMap(std::vector<int>{}),    false);
```

Qt

```C++
QCOMPARE(_::isMap(QMap<int,int>{}),       true);
QCOMPARE(_::isMap(QVariantMap{}),         true);

QCOMPARE(_::isMap(QVector<int>{ }),       false);
QCOMPARE(_::isMap(QList<int>{ }),         false);
```
