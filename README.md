## Underline
[![Build Status](https://www.travis-ci.org/benlau/underline.svg?branch=master)](https://www.travis-ci.org/benlau/underline)
[![Build status](https://ci.appveyor.com/api/projects/status/p6jsldfoj73ep630?svg=true)](https://ci.appveyor.com/project/benlau/underline)

Underline is a C++ utility library provides useful functional programming helpers like the lodash.js.

An online demonstration for STL library is available at https://repl.it/@benlau/underline

Features
======

 1. Designed for user-friendly error handling. Get rid of horrific and misleading template compilation error messages
 1. C++11 Compliant Coding.
 1. Support C++14 generic lambda function (using auto as a parameter)
 1. Support iteratee functions with one to more arguments.
 1. Support Qt types, but it still compilable for non-Qt applications.
 1. Single Header Library

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

Direct Download
```
wget https://raw.githubusercontent.com/benlau/underline/master/src/cpp/underline.h
```

QPM
```
qpm install com.github.benlau.underline
```

Programming Guide
=============

1) Use "auto" type



API
===

 * [Object Manipulation](#object-manipulation)
 * [Type Check](#type-checker)

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

```C++
template <typename Number>
Number _::clamp(Number number, Number lower, Number upper)
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
template <typename Collection, typename Iteratee>
Map countBy(const Collection& collection, Iteratee iteratee)
```

Iterates all the elements in the collection container and pass to the iteratee function, then count the number of the occurrence of the result. The iteratee function takes only one argument. [value]

The return is a Map object in a type either of std::map or QMap. The actual type chosen depends on the collection container. If it is a Qt container class, it will be a QMap. Otherwise, it will be an std::map. The key type is the same as the return of the iteratee, and the value type is an integer storing the counting.

Arguments:

 * collection: The input source. Check [_::isCollection](#iscollection) function for the supported types
 * iteratee:  The iteratee function to transform the element in the collection to a key value.

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

filter
----

```C++
template <typename Collection, typename Predicate>
Collection filter(const Collection& collection, Predicate predicate);
```
Creates a new collection object obtaining the elements from the input collection which was passed to the predicate function returning true. The predicate may take one to three arguments [value [index [collection]]]

Arguments
 * collection: The input collection.  [_::isCollection](#iscollection) function for the supported types
 * predicate: The predicate function returns true or false

Return
 * A newly created collection object

Example
```C++
```

forEach
-------

```C++
template <typename Collection, typename Iteratee>
inline const Collection& forEach(const Collection& collection, Iteratee iteratee);
```

Iterates all the elements in the collection container and calls the iteratee function.  The iteratee function can take from one to three arguments [value [index [collection]]]. If the iteratee function returns a false value, the looping stop immediately.

Arguments:
 * object: The source object. Check [_::isCollection](#iscollection) function for the supported types.
 * iteratee: The function invoked per iteration.

Return:
 * object: The input object

Example
```C++
```

forIn
-----

```
template <typename KeyValueType, typename Iteratee>
KeyValueType& _::forIn(const KeyValueType& object, Iteratee iteratee)
```

Iterates all the string key properties of an object and calls the iteratee function. The iteratee function is invoked with one to three arguments: [value, [key, [object]]].

Arguments:

 * object: The source object. Check [_::isKeyValueType](#iskeyvaluetype) function for the supported types.
 * iteratee: The function invoked per iteration.

Returns:

 * object

Example
```
```

get
---

```C++
template <typename isQtMetable>
QVariant _::get(const isQtMetable& object, const QString &path,QVariant defaultValue = QVariant());
```

Obtain the value from the source object at the given path. If the path does not exist, it returns the default value.

Arguments
 * object: The source check. Check [_::isQtMetable](#isqtmetable) for the supported types
 * path (QString): The path of the property to get
 * defaultValue(QVariant): The default value to return if the path does not exist.

Return:
 * value: The value in the given path. If the path doesn't exist, it will return the defaultValue.

Example (Qt):

```C++
// Obtain the objectName property from object's parent
QVariant property = _::get(/* QObject* */ object, "parent.objectName");
```

keyBy
-----

```C++
template <typename Collection,  typename Iteratee>
Map keyBy(const Collection& collection, Iteratee iteratee)
```

Create a Map container class where the keys are the result of running iteratee function over the all the elements in the collection. The corresponding value of each key is the last element responsible for generating the key. The iteratee function can take at most one argument which is the element in the collection.

The actual type of the Map container is determined by the Collection class. If it is an STL class, then an std::map is chosen. Qt container types return QMap.

Arguments:
 * collection: The input source. Check [_::isCollection](#iscollection) function for the supported types
 * iteratee:  The iteratee function to transform the element in the collection to a key value.

Return
 * Map

Example
```C++
```

map
----

```C++
template <typename Collection, typename Iteratee>
_::map(Collection collection, Iteratee iteratee)
```

Iterate all the elements of the collection container and call the iteratee function. It creates a new collection obtaining the result. The iteratee is invoked with one to three arguments: [value, [index, [collection]]].

Arguments:

 * collection: The input collection collection. Check [_::isCollection](#iscollection) for the supported types.
 * iteratee: The function invoked per iteration.

Returns:
 * (Collection) The new mapped collection. The container type is the same as the input collection.

**Example:**

STD:
```C++
QList<int> output1 = _::map(QList<QString>(){"1","2","3"},  [](auto item) { return item.toInt();});

QVector<int> output2 = _::map(QVector<QString>() {"1","2","3"}, [](auto item, int index) { return item.toInt();});

QVector<int> output3 = _::map(std::vector<QString>(){"1","2","3"}, [](auto item, int index, auto collection) { return item.toInt();});
```

omit
----

```C++
template <typename QtMetable>
QVariantMap omit(const QtMetable& source, const QStringList& paths);

template <typename QtMetable>
QVariantMap omit(const QtMetable& source, const QString& path);
```

Creates a new object which is a clone of the source, but the properties listed in the paths are omitted.

Arguments

 * source: The source object
 * paths: The property paths to omit

Returns

 * QVariantMap: Returns the new object.

pick
---

```C++
template <typename QtMetable>
QVariantMap _::pick(const QtMetable& object, QStringList &paths);

template <typename QtMetable>
QVariantMap _::pick(const QtMetable& object, QString path);
```

Creates a new object which is a clone of the source object but only the properties listed in the paths are picked.

Arguments

 * source: The source object
 * paths: The property paths to pick

Returns

 * QVariantMap: Returns the new object.

Example
```C++
```

set
---

```C++
template <typename QtMetable>
void _::set(QtMetable &object, const QString &path, const QVariant &value)
```

Set the property in the destination object on the given path. If the object is a QVariantMap / QJSValue and a portion of path doesn't exist, it is created.

Arguments

 * source: The input object. Check [_::isQtMetable](#isqtmetable) for the supported types.
 * paths: The property paths to set
 * value: The property value to be set

Return
 * object: The destination object

Example:

STL
```C++
    // Input: {"a":1,"b":2,"c":{"d":"3"}}
    QVariantMap object = {{"a", 1}, {"b", 2.0}, {"c", QVariantMap{{"d", "3"}}}};

    _::set(object, "b", 2.1);
    _::set(object, "c.e", "3.1");
    _::set(object, "f.g", "4.1");

    ASSERT_EQ(object["b"].toDouble() , 2.1);
    ASSERT_EQ(object["c"].toMap()["d"].toString() , QString("3"));
    ASSERT_EQ(object["c"].toMap()["e"].toString() , QString("3.1"));
    ASSERT_EQ(object["f"].toMap()["g"].toString() , QString("4.1"));

    // Result: {"a":1,"b":2.1,"c":{"d":"3","e":"3.1"},"f":{"g":"4.1"}}
```
Qt
```C++
QObject* object = new QObject();
QObject* subObject = new QObject(object);
object->setProperty("value", QVariant::fromValue<QObject*>(subObject));  // object: {"objectName":"","value":{"objectName":""}}

_::set(object, "value.objectName", "subObject"); //object: {"objectName":"","value":{"objectName":"subObject"}}

ASSERT_EQ(subObject->objectName(), QString("subObject"));
```

toCollection
--------

```C++
template <typename Map>
Collection toCollection(const Map& map)
```

Convert the Map object into a Collection object discarding the key type. The actual type is chosen by the input type. If it is a Qt container class, it will be a QList, otherwise, it is a std::vector.

Arguments

 * map: A map container type

Return
 * collection: A collection object containing the value type from the Map object

Examples:

STL
```C++
        std::map<int, int> map{{0,1}, {2,3}, {4,5}};

        std::vector<int> collection = _::toCollection(map);

        QCOMPARE(static_cast<int>(collection.size()), 3);
        QCOMPARE(collection, (std::vector<int>{1,3,5}));
```
QT
```C++
        QMap<int, int> map{{0,1}, {2,3}, {4,5}};

        QList<int> collection = _::toCollection(map);

        QCOMPARE(collection.size(), 3);
        QCOMPARE(collection, (QList<int>{1,3,5}));
```

range
----

```C++
template <typename Collection, typename V1>
Collection range(V1 end) ;

template <typename Collection, typename V1, typename V2>
Collection range(V1 start, V2 end);

template <typename Collection, typename V1, typename V2, typename V3>
Collection range(V1 start, V2 end, V3 step);

template <typename ...Args>
QList<int> rangeQ(Args ...args);

template <typename ...Args>
std::vector<int> rangeS(Args ...args);
```

Create a Collection object of numbers and fill the elements with the range from the start up to, but not including, the end value. _::range is a template function couldn't deduce the output type from the input value. Users have to specify the return type.

rangeS / rangeQ are the non-template version of _::range where the return type is defined.

Arguments:

 * start: The starting value. If it is not specific, it will be 0.
 * end: The ending value.
 * step: The value to increment or decrement. The default value is 1. It may set to a negative value.

Return:
* collection

Example
```C++
 _::range<std::vector<int>>(5); // std::vector<int>{0,1,2,3,4}
 _::range<std::vector<int>>(-5); // std::vector<int>{0,-1,-2,-3,-4}
_::range<std::vecto<int>>(1,5),  // std::vector<int>{1,2,3,4}
_::range<std::vecto<int>>(0,20,5),  // std::vector<int>{0,5,10,15}
```

reduce
------

```C++
template <typename Collection, typename Reducer, typename Accumulator>
Accumulator reduce(const Collection& collection, Reducer reducer, Accumulator accumlator)
```

Calls the reducer function on each member of the collection and return the accumulator as the single output.  The reducer function could take from one to four parameters:

 1.  The accumulator
 1. Current Value from the collection
 1. Current Index
 1. The source collection

The reducer function must return a value with type same as the accumulator.

Arguments

 1. collection: The input. Check [_::isCollection](#iscollection) for supported types
 2. reducer: The reducer function
 3. accumulator

Return:

 * accumulator

Example
```C++
```

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

Object Manipulation
===

merge
------

Qt-Only API

```c++
template <typename QtMetable1, typename QtMetable2>
QtMetable1& merge(QtMetable1& object, QtMetable2& source)
```

Iterates all the properties from the source object then copy to the destination object.  If the property exists on the destination object, it will call itself recursively for merging.

Arguments:
 * object: The destination object. Check [_::isQtMetable](#isqtmetable) for the supported types.
 * source: The source object. Check [_::isQtMetable](#isqtmetable) for the supported types.

Return:
 * object: The destination object

Examples
```C++
    QVariantMap source = _::parse("{\"a\":1,\"b\":{\"c\":3}}"); // "{"a":1,"b":{"c":3}}"
    QVariantMap object = _::parse("{\"b\":{\"d\":4},\"e\":5}"); // "{"b":{"d":4},"e":5}"

    _::merge(object, source); // "{"a":1,"b":{"c":3,"d":4},"e":5}"

    ASSERT_EQ(_::stringify(object), QString("{\"a\":1,\"b\":{\"c\":3,\"d\":4},\"e\":5}"));
```

```C++
//Header
class Gadget {
    Q_GADGET
    Q_PROPERTY(int     value1 MEMBER value1)
    Q_PROPERTY(double  value2 MEMBER value2)
    Q_PROPERTY(QString value3 MEMBER value3)

public:
    int value1 = 1;
    double value2 = 2.1;
    QString value3 = "3";
};

//Source
    Gadget source;
    QVariantMap object;

    _::merge(object, source);

    ASSERT_EQ(_::stringify(object),
              QString("{\"value1\":1,\"value2\":2.1,\"value3\":\"3\"}"));
    // "{"value1":1,"value2":2.1,"value3":"3"}"
```

Type Checker
============

isCollection
------

```C++
bool _::isCollection(const T&)
bool _::isCollection<T>()
```

It is a static type checker to validate is the input type classified as a valid collection class supported by _.   You rarely need to use this function directly.

Example:

```C++
QCOMPARE(_::isCollection(std::vector<int>{}),    true);
QCOMPARE(_::isCollection(QVector<int>{ }),       true);
QCOMPARE(_::isCollection(QList<int>{ }),         true);
QCOMPARE(_::isCollection(QVariantList{ }),       true);
QCOMPARE(_::isCollection(QString{ }),            true);

QCOMPARE(_::isCollection(std::map<bool,int>{}),  false);
QCOMPARE(_::isCollection(QMap<int,int>{}),       false);
QCOMPARE(_::isCollection(10),                    false);
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
