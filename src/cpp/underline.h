/** Underline
 *
 * @version 0.0.1
 * @author Ben Lau
 * @license Apache-2.0
 */
#pragma once
#include <functional>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <unordered_map>

#ifdef QT_CORE_LIB
#include <QtCore>
#include <QtGlobal>
#include <QList>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
#include <QObject>
#include <QMetaObject>
#include <QJsonDocument>
#endif

#ifdef QT_QUICK_LIB
#include <QJSValue>
#include <QJSValueIterator>
#endif

/** Design Principle and Known Issues

1. Get rid of horrific template compilation error message

2. Don't use constexpr together with std::enable_if.

c++ - enable_if not working in Visual Studio when using a constexpr function as argument - Stack Overflow
https://stackoverflow.com/questions/46144103/enable-if-not-working-in-visual-studio-when-using-a-constexpr-function-as-argume

 */
#define _underline_iteratee_mismatched_error "Mismatched argument types in the iteratee function. Please validate the number of argument and their type."
#define _underline_predicate_mismatched_error "Mismatched argument types in the predicate function. Please validate the number of argument and their type."
#define _underline_predicate_return_type_mismatch_error "The return type of predicate function must be bool"
#define _underline_input_type_is_not_array "Invalid Argument Type. The input should be an valid Collection class where _::isCollection() returns true. "
#define _underline_iteratee_void_ret_error "The return type of iteratee function cannot be void"
#define _underline_invalid_argument_type_it_must_be_a_valid_key_value_type "Invalid argument type. It should be a valie Key-Value-Type. Check the document of _::isKeyValueType for the list of supported types"
#define _underline_qjsvalue_set_error "It should only modify the value of a QJSValue object by another QJSValeu"
#define _underline_default_value_type_doesnot_match_with_collection_value_type "The default value type does not match with the value type of collection."

#define _underline_static_assert_is_collection(prefix, type) \
    static_assert(_::Private::is_static_collection<type>::value, prefix _underline_input_type_is_not_array)

#define _underline_static_assert_is_map(prefix, type) \
    static_assert(_::Private::is_map<type>::value, prefix "The expected input is an valid Map container class, where _::isMap() returns true.")

#define _underline_static_assert_is_key_value_type(prefix, type) \
    static_assert(_::Private::is_key_value_type<type>::value, prefix _underline_invalid_argument_type_it_must_be_a_valid_key_value_type)

#define _underline_static_assert_is_static_qt_metable(func, type) \
    static_assert(_::Private::is_static_qt_metable<type>::value, func ": Invalid argument type. It should be a QtMetable type. Check the document of _::isQtMetable for the list of supported types.")

#define _underline_static_assert_is_iteratee_invokable(prefix, value) \
    static_assert(value, prefix _underline_iteratee_mismatched_error)

#define _underline_static_assert_is_iteratee_not_void(prefix, value) \
    static_assert(value, prefix _underline_iteratee_void_ret_error)

#define _underline_static_assert_is_object_source_key_matched(prefix, object, source) \
    static_assert(Private::is_custom_convertible<source,object>::value, prefix "The key type of 'source' argument cannot convert to the key type of 'object' argument.")

#define _underline_static_assert_is_object_source_value_matched(prefix, object, source) \
    static_assert(Private::is_custom_convertible<source,object>::value, prefix "The value type of 'source' argument cannot convert to the value type of 'object' argument.")

#define _underline_static_assert_is_object_a_qt_metable(prefix, object) \
    static_assert(Private::is_static_qt_metable<object>::value, prefix "Invalid object type. It should be a QtMetable type. Check the document of _::isQtMetable for supported types.")

#ifndef _underline_debug
#define _underline_debug(msg)
#endif

#define _declare_underline_has(name, testMethod, retType) \
        template <typename T> \
        struct has_##name { \
            template <typename Type> \
            static inline auto test(int) -> typename std::enable_if<std::is_convertible<testMethod, retType>::value, bool>::type; \
            template <typename> \
            static inline auto test(...) -> Undefined; \
            enum { \
                value = !std::is_same<decltype(test<remove_cvref_t<T>>(0)), Undefined>::value \
            }; \
        }; \

/// Register rebind_to_map and test_is_collection
#define _underline_register_rebind_to_map(CollectionType, MapType) \
    namespace _ { \
        namespace Private { \
            template <class ValueType, class NewKeyType, class NewValueType> \
            struct convert_collection_to_map<CollectionType<ValueType>, NewKeyType, NewValueType> { \
                typedef MapType<avoid_void_t<NewKeyType>, avoid_void_t<NewValueType>> type; \
            }; \
        } \
    }

#define _underline_register_std_type(T) \
    namespace _ { namespace Private { \
        template <typename ...Args> struct is_std_type<T<Args...> > : std::true_type {}; \
    } }

#define _underline_register_qt_container_type(T) \
    namespace _ { namespace Private { \
        template <typename ...Args> struct is_qt_type<T<Args...> > : std::true_type {}; \
    } }

#define _underline_register_qt_non_container_type(T) \
    namespace _ { namespace Private { \
        template <> struct is_qt_type<T> : std::true_type {}; \
    } }

namespace _ {

    namespace Private {

        /// An Undefined class is a default return type of invalid function to keep compiler happy
        class Undefined {
        };

        /// A dummy class to keep compiler happy
        template <typename Value>
        class NullArray {
            template <typename ...Args>
            void reserve(Args...) {}

            template <typename ...Args>
            void push_back(Args...) {}
        };

        /// A dummy class to keep compiler happy
        template <typename Key, typename Value>
        class NullMap {
        };

#ifndef QT_CORE_LIB
        class QObject{};
        template <typename Key, typename Value>
        class QMap {
        public:
            class const_iterator{};
            class iterator { };
        };
        class QString{};
        class QVariant{
        public:
            QMap<QString, QVariant> toMap() const;
        };
        class QMetaProperty{
        public:
            QString name() const;
            template <typename ...Args> QVariant read(Args...) const;
            template <typename ...Args> QVariant readOnGadget(Args...) const;
        };
        class QMetaObject{
        public:
            int propertyCount() const;
            QMetaProperty property(int) const;
        };
        class QByteArray{};
        class QVariantMap: public QMap<QString,QVariant>{};
        template <typename V>
        class QList {};
#endif
#ifndef QT_QUICK_LIB
        class QJSValue{
        public:
            inline QJSValue property(QString) const { return QJSValue();}
            inline void setProperty(QString, QJSValue) {}
            inline bool isUndefined() const {return true;}
        };
        class QJSValueIterator{
        public:
            template <typename ...Args> inline bool hasNext(Args...);
            template <typename ...Args> void next(Args...);
            template <typename ...Args> void value(Args...);
            template <typename ...Args> void name(Args...);
        };
#endif

        class GadgetContainer { // A Wrapper of Qt's Gadget object
            public:
                const QMetaObject* metaObject = nullptr;
                void* data = nullptr;
                const void* constData = nullptr;
        };

        class ConstCharContainer {
        public:
            const char* data = nullptr;
            QByteArray bytes;
        };

        template <typename T>
        using remove_cvref_t = typename std::remove_reference<typename std::remove_cv<T>::type>::type;

        _declare_underline_has(reserve, decltype(std::declval<Type>().reserve(0)), void)

        _declare_underline_has(push_back, decltype(std::declval<Type>().push_back(std::declval<typename remove_cvref_t<Type>::value_type>())), void)

        _declare_underline_has(static_meta_object, typename std::remove_cv<decltype(std::remove_pointer<Type>::type::staticMetaObject)>::type, QMetaObject)

        _declare_underline_has(operator_round_backets_int, decltype(std::declval<Type>()[0]), typename std::remove_cv<Type>::type::value_type)

        _declare_underline_has(operator_round_backets_key, decltype(std::declval<Type>()[std::declval<typename std::remove_cv<Type>::type::key_type>()]),
                                                           typename std::remove_cv<Type>::type::mapped_type)

        _declare_underline_has(mapped_type,typename std::remove_cv<Type>::type::mapped_type,typename std::remove_cv<Type>::type::mapped_type)

        _declare_underline_has(key_type,typename std::remove_cv<Type>::type::key_type,typename std::remove_cv<Type>::type::key_type)

        template <typename...>
        using std_true_type = std::true_type;

        template <typename F>
        auto resolve_is_lambda(int) -> std_true_type<decltype (&F::operator())> {}

        template <typename>
        auto resolve_is_lambda(...) -> void {}

        template <typename F>
        struct is_lambda : std::integral_constant<
                    bool,
                    std::is_same<decltype(resolve_is_lambda<F>(0)), std::true_type>::value
        > {};

        template <typename F>
        struct is_callable : std::integral_constant<bool, is_lambda<F>::value || std::is_function<F>::value> {};

        template <typename T>
        using pointer_or_reference_t = typename std::conditional<std::is_pointer<T>::value, T, T&>::type;

        template <typename T>
        inline auto is_null_ptr(T&) -> typename std::enable_if<!std::is_pointer<T>::value, bool>::type {
            return false;
        }

        template <typename T>
        inline auto is_null_ptr(T& t) -> typename std::enable_if<std::is_pointer<T>::value, bool>::type {
            return t == nullptr;
        }

        template <typename V1, typename V2>
        inline auto copy_if_same_type(V1 &v1, const V2 &v2) -> typename std::enable_if<std::is_same<V1,V2>::value>::type {
            v1 = v2;
        }

        template <typename V1, typename V2>
        inline auto copy_if_same_type(V1 &, const V2&) -> typename std::enable_if<!std::is_same<V1,V2>::value>::type {
        }

        template <typename From, typename To>
        inline auto convertTo(const From&,  To&) -> typename std::enable_if<!std::is_convertible<From,To>::value, Undefined>::type { return Undefined(); }

        template <typename From, typename To>
        inline auto convertTo(const From& from, To& to) -> typename std::enable_if<std::is_convertible<From,To>::value, void>::type {
            to = from;
        }
#ifdef QT_CORE_LIB
        inline void convertTo(const QVariantMap& from, QVariant& to) {
            to = static_cast<QVariant>(from);
        }
#endif
#ifdef QT_QUICK_LIB
        inline void convertTo(const QJSValue& from, QVariant& to) {
            to = from.toVariant();
        }
#endif

        template <typename V1, typename V2>
        inline void copy_or_convert(V1& v1, const V2& v2) {
            if (std::is_same<V1,V2>::value) {
                copy_if_same_type(v1, v2);
            } else {
                convertTo(v2, v1);
            }
        }

        template <typename From, typename To>
        struct is_custom_convertible : std::integral_constant<int,std::is_convertible<From, To>::value || !std::is_same<Undefined, decltype(convertTo(std::declval<From>(), std::declval<To&>()))>::value> {};

        template <typename T> // Avoid returning a void, change it to Undefined
        using avoid_void_t = typename std::conditional<std::is_same<T,void>::value, Undefined, T>::type;

        /* BEGIN is_xxx */

        template <typename T>
        struct is_static_collection: std::integral_constant<int,
            std::is_class<T>::value &&
            has_push_back<T>::value &&
            has_operator_round_backets_int<T>::value &&
            has_reserve<T>::value
        >{};

        template <typename T>
        struct is_map: std::integral_constant<int,
            has_key_type<T>::value &&
            has_mapped_type<T>::value &&
            has_operator_round_backets_key<T>::value
         >{};

        template <typename Object>
        struct is_qobject: std::integral_constant<int,std::is_convertible<typename std::add_pointer<typename std::remove_pointer<remove_cvref_t<Object>>::type>::type, const QObject*>::value> {};

        template <typename T> struct is_gadget: std::integral_constant<int, has_static_meta_object<T>::value && ! is_qobject<T>::value> {};

        template <typename T> struct is_qjsvalue: std::integral_constant<int,std::is_same<remove_cvref_t<T>, QJSValue>::value> {};

        template <typename T> struct is_qvariant: std::integral_constant<int,std::is_same<remove_cvref_t<T>, QVariant>::value> {};

#ifdef QT_QUICK_LIB
        template <typename T> struct is_real_qjsvalue: is_qjsvalue<T> {};
#else
        template <typename T> struct is_real_qjsvalue: std::integral_constant<int,is_qjsvalue<T>::value && ! std::is_same<remove_cvref_t<T>, _::Private::QJSValue>::value> {};
#endif

        template <typename T> struct is_qt_any_type : std::integral_constant<int, std::is_same<T,QJSValue>::value || std::is_same<T,QVariant>::value> {};

        template <typename ...Args>
        struct is_std_type : std::false_type {};

        template <typename T>
        struct is_std_map : std::integral_constant<int, is_std_type<T>::value && is_map<T>::value> {};

        template <typename ...Args>
        struct is_qt_type: std::false_type {};

        /* END is_xxx */

        template <typename T, typename V>
        using map_to_collection_rebinder_t = typename std::conditional<
            is_qt_type<T>::value, QList<V>, std::vector<V>
        >::type;

        template <typename T>
        auto inline cast_to_pointer(const T& value) -> typename std::enable_if<std::is_pointer<T>::value, const T&>::type {
            return value;
        }

        template <typename T>
        auto inline cast_to_pointer(const T& value) -> typename std::enable_if<!std::is_pointer<T>::value, const T*>::type {
            return &value;
        }

        template <typename T>
        auto inline cast_to_pointer(T& value) -> typename std::enable_if<std::is_pointer<T>::value, T&>::type {
            return value;
        }

        template <typename T>
        auto inline cast_to_pointer(T& value) -> typename std::enable_if<!std::is_pointer<T>::value, T*>::type {
            return &value;
        }

        template <typename T>
        inline auto cast_to_qobject(const T& ) -> typename std::enable_if<!std::is_same<T, QVariant>::value && !is_qobject<T>::value, QObject*>::type {
            return nullptr;
        }

#ifdef QT_CORE_LIB
        template <typename T>
        inline auto cast_to_qobject(T* t) -> typename std::enable_if<is_qobject<T>::value, QObject*>::type {
            return qobject_cast<QObject*>(t);
        }

        template <typename T>
        inline auto cast_to_qobject(const T* t) -> typename std::enable_if<is_qobject<T>::value, const QObject*>::type {
            return qobject_cast<const QObject*>(t);
        }

        inline auto cast_to_qobject(const QVariant& t) -> QObject* {
            QObject* res = nullptr;
            if (t.canConvert<QObject*>()) {
                res = qvariant_cast<QObject*>(t);
            }
            return res;
        }
#endif
#ifdef QT_QUICK_LIB
        inline auto cast_to_qobject(const QJSValue& t) -> QObject* {
            QObject* res = nullptr;
            if (t.isQObject()) {
                res = t.toQObject();
            }
            return res;
        }
#endif

        template <typename T>
        inline QVariantMap cast_to_qvariantmap(const T&) {
            return QVariantMap();
        }

        template <typename T>
        inline auto can_cast_to_qvariantmap(const T&) -> typename std::enable_if<!is_qt_any_type<T>::value, bool>::type {
            return false;
        }

#ifdef QT_CORE_LIB
        inline auto cast_to_qvariantmap(const QVariant& t) -> QVariantMap {
            return t.toMap();
        }

        inline auto cast_to_qvariantmap(const QVariantMap& map) -> QVariantMap {
            return map;
        }

        inline auto can_cast_to_qvariantmap(const QVariant& t) -> bool {
            return t.type() == QVariant::Map;
        }

        inline auto can_cast_to_qvariantmap(const QVariantMap& ) -> bool {
            return true;
        }
#endif

#ifdef QT_QUICK_LIB
        inline auto cast_to_qvariantmap(const QJSValue& t) -> QVariantMap {
            return t.toVariant().toMap();
        }
        inline auto can_cast_to_qvariantmap(const QJSValue& t) -> bool {
            return t.isObject();
        }
#else
        inline auto can_cast_to_qvariantmap(const QJSValue&) -> bool {
            return false;
        }

        inline auto cast_to_qvariantmap(const QJSValue&) -> QVariantMap {
            return QVariantMap();
        }
#endif

        template <typename T>
        inline auto cast_to_gadget_container(const T&) -> typename std::enable_if<!is_qvariant<T>::value && !is_gadget<T>::value, GadgetContainer>::type {
            return GadgetContainer();
        }

#ifdef QT_CORE_LIB
        template <typename T>
        inline auto cast_to_gadget_container(T& object) -> typename std::enable_if<is_gadget<T>::value, GadgetContainer>::type {
            GadgetContainer container;
            container.metaObject = &T::staticMetaObject;;
            container.data = &object;
            container.constData = &object;
            return container;
        }

        template <typename T>
        inline auto cast_to_gadget_container(const T& object) -> typename std::enable_if<is_gadget<T>::value, GadgetContainer>::type {
            GadgetContainer container;
            container.metaObject = &T::staticMetaObject;;
            container.constData = &object;
            return container;
        }

        inline GadgetContainer cast_to_gadget_container(const QVariant &v) {
            GadgetContainer gadget;

            QMetaType type (v.userType());
            const QMetaObject* metaObject = type.metaObject();
            if (metaObject == nullptr || metaObject->inherits(&QObject::staticMetaObject)) {
                return gadget;
            }

            gadget.metaObject = metaObject;
            gadget.constData = v.constData();
            return gadget;
        }

        inline GadgetContainer cast_to_gadget_container(QVariant &v) {
            const QVariant& cv = v;
            GadgetContainer gadget = cast_to_gadget_container(cv);
            if (gadget.metaObject == nullptr) {
                return gadget;
            }
            gadget.data = v.data(); /* QVariant::data() is a non-documented API and not used by any inline function. Reference: QTBUG-35510 */
            return gadget;
        }
#endif

        inline ConstCharContainer cast_to_const_char_container(const char* value) {
            ConstCharContainer container;
            container.data = value;
            return container;
        }

#ifdef QT_CORE_LIB
        template <typename T>
        inline auto cast_to_const_char_container(const T& string) -> typename std::enable_if<std::is_same<T, QString>::value, ConstCharContainer>::type {
            ConstCharContainer container;
            container.bytes = string.toUtf8();
            container.data = container.bytes.constData();
            return container;
        }

        template <typename T>
        inline auto cast_to_const_char_container(const T& string) -> typename std::enable_if<std::is_same<T, std::string>::value, ConstCharContainer>::type {
            ConstCharContainer container;
            container.data = string.c_str();
            return container;
        }
#endif

        template <typename T>
        inline auto cast_to_qstring(const T&) -> QString {
            return QString();
        }

#ifdef QT_CORE_LIB
        inline auto cast_to_qstring(const QString &str) -> QString {
            return str;
        }
        inline auto cast_to_qstring(const char* str) -> QString {
            return QString(str);
        }
#endif
#ifdef QT_QUICK_LIB
        inline auto cast_to_qstring(const QJSValue &value) -> QString {
            return value.toString();
        }
#endif
        template <typename T>
        inline auto cast_to_qjsvalue(T&) -> typename std::enable_if<!is_qjsvalue<T>::value, QJSValue>::type {
            return QJSValue();
        }

        template <typename T>
        inline auto cast_to_qjsvalue(T& v) -> typename std::enable_if<is_qjsvalue<T>::value, T>::type {
            return v;
        }

        /* END of cast_to_xxx */

#ifdef QT_CORE_LIB
        class QtMetaObject {
        public:
            class List {};
            std::function<unsigned int(const QVariant&)> count;
            std::function<QVariant(const QVariant&, unsigned index)> get;
            std::function<QVariant()> create;
            std::function<QVariantList(const QVariant&)> toVariantList;
            std::function<QVariant(const QVariant&)> fromVariantList;
        };

        template <typename T>
        class QtMetaObjectTable {
        public:
            static QMap<int, QtMetaObject>& storage() {
                static QMap<int, QtMetaObject> storage;
                return storage;
            }
        };

        inline QVariant convertQVariant(const QVariant& v, int userType) {
            if (v.userType() == userType) {
                return v;
            }
            auto listTable = QtMetaObjectTable<Private::QtMetaObject::List>::storage();
            if (listTable.contains(userType)) {
                return listTable[userType].fromVariantList(v);
            }
            return v;
        }
#endif

        /* BEGIN key_value_xxx */

        template <typename ...Args>
        struct _key_value_info {
            enum {
                is_key_value_type = false
            };
            using key_type = Undefined;
            using value_type = Undefined;
        };

        template <typename T>
        struct _key_value_info<T,
                typename std::enable_if<has_static_meta_object<T>::value || std::is_same<T, GadgetContainer>::value
                , std::true_type>::type> {
            enum { is_key_value_type = true};
            using key_type = QString;
            using value_type = QVariant;
        };

        template <typename T>
        struct _key_value_info<T, typename std::enable_if<std::is_same<T, QJSValue>::value, std::true_type>::type> {
            enum { is_key_value_type = true};
            using key_type = QString;
            using value_type = QJSValue;
        };

        template <typename T>
        struct _key_value_info<T, typename std::enable_if<is_map<T>::value,  std::true_type>::type> {
            enum { is_key_value_type = true};
            typedef typename T::key_type key_type;
            typedef typename T::mapped_type value_type;
        };

        template <typename T>
        struct _key_value_info<T, typename std::enable_if<is_qvariant<T>::value,  std::true_type>::type> {
            enum { is_key_value_type = false};
            using key_type = QString;
            using value_type = QVariant;
        };

        template <typename T>
        struct key_value_info : _key_value_info<T, std::true_type>{};

        template <typename Meta, typename Key>
        inline auto key_value_read(const Meta& meta, const Key& key) -> typename std::enable_if<is_gadget<Meta>::value, QVariant>::type {
            auto ptr = cast_to_pointer<Meta>(meta);
            auto k = cast_to_const_char_container(key);
            auto metaObject = ptr->staticMetaObject;
            int index = metaObject.indexOfProperty(k.data);
            if (index < 0 ) {
                return QVariant();
            }
            auto property = metaObject.property(index);
            return property.readOnGadget(ptr);
        }

        template <typename Meta, typename Key>
        inline auto key_value_read(const Meta& meta, const Key& key) -> typename std::enable_if<is_qobject<Meta>::value, QVariant>::type {
            auto k = cast_to_const_char_container(key);
            return meta->property(k.data);
        }

        template <typename Meta, typename Key>
        inline auto key_value_read(const Meta& meta, const Key& key) -> typename std::enable_if<is_qjsvalue<Meta>::value, QJSValue>::type {
            return meta.property(cast_to_qstring(key));
        }

        template <typename Meta, typename Key, typename Value>
        inline auto key_value_write(Meta& meta, const Key& key, const Value& value) -> typename std::enable_if<is_gadget<Meta>::value, bool>::type {
            auto ptr = cast_to_pointer<Meta>(meta);
            auto metaObject = ptr->staticMetaObject;
            auto k = cast_to_const_char_container(key);

            int index = metaObject.indexOfProperty(k.data);
            if (index < 0 ) {
                return false;
            }
            auto property = metaObject.property(index);
            auto convertedValue = convertQVariant(value, property.userType());
            property.writeOnGadget(ptr, convertedValue);
            return true;
        }

        template <typename Meta, typename Key, typename Value>
        inline auto key_value_write(Meta& meta, const Key& key, const Value& value) -> typename std::enable_if<is_qobject<Meta>::value, bool>::type {
            auto k = cast_to_const_char_container(key);
            auto metaObject = meta->staticMetaObject;
            int index = metaObject.indexOfProperty(k.data);
            if (index < 0 ) {
                return meta->setProperty(k.data, value);
            }
            auto property = metaObject.property(index);
            auto convertedValue = convertQVariant(value, property.userType());
            return property.write(meta, convertedValue);
        }

        template <typename Meta, typename Key, typename Value>
        inline auto key_value_write(Meta& meta, const Key& key, const Value& value) -> typename std::enable_if<is_qjsvalue<Meta>::value, bool>::type {
            meta.setProperty(cast_to_qstring(key), value);
            return true;
        }

        template  <typename Ref> // Create an empty object according to the ref type
        inline auto contruct_default_object(const Ref&) -> Undefined {
            return Undefined();
        }

        template  <typename ...Args> // Create an empty collection according to the ref type. It may take multiple arguments for handling QJSValue
        inline auto construct_default_collection(Args ...) -> std::vector<Undefined> {
            return std::vector<Undefined>();
        }

#ifdef QT_CORE_LIB
        template <typename Meta, typename Key>
        inline auto key_value_read(const Meta& gadget, const Key& key) -> typename std::enable_if<std::is_same<GadgetContainer, Meta>::value, QVariant>::type {
            if (gadget.constData == nullptr) {
                return QVariant();
            }
            auto k = cast_to_const_char_container(key);
            int index = gadget.metaObject->indexOfProperty(k.data);
            if (index < 0) return QVariant();

            QMetaProperty prop = gadget.metaObject->property(index);

            return prop.readOnGadget(gadget.constData);
        }

        template <typename Meta, typename Key, typename Value>
        inline auto key_value_write(Meta& gadget, const Key& key, const Value& value) -> typename std::enable_if<std::is_same<GadgetContainer, Meta>::value, bool>::type {
            if (gadget.data == nullptr) {
                return false;
            }
            auto k = cast_to_const_char_container(key);
            int index = gadget.metaObject->indexOfProperty(k.data);
            if (index < 0) return false;

            QMetaProperty prop = gadget.metaObject->property(index);
            prop.writeOnGadget(gadget.data, value);
            return true;
        }

        inline auto contruct_default_object(const QVariant&) -> QVariantMap {
            return QVariantMap();
        }

        inline auto contruct_default_object(const QVariantMap&) -> QVariantMap {
            return QVariantMap();
        }

        inline auto contruct_default_object(QObject*) -> QVariantMap {
            return QVariantMap();
        }

        template <typename ...Args> inline auto construct_default_collection(const QVariant&, Args ...) -> QVariantList {
            return QVariantList();
        }

        template <typename ...Args> inline auto construct_default_collection(const QVariantMap&, Args ...) -> QVariantList {
            return QVariantList();
        }

        template <typename ...Args> inline auto construct_default_collection(QObject*, Args...) -> QVariantList {
            return QVariantList();
        }

#endif

#ifdef QT_QUICK_LIB
        inline auto contruct_default_object(const QJSValue& value) -> QJSValue {
            auto prototype = value.prototype();
            auto p = prototype;

            while (!p.isNull()) {
                prototype = p;
                p = p.prototype();
            }

            if (prototype.isNull()) {
                return QJSValue();
            }

            return prototype.property("constructor").callAsConstructor();
        }

        template <typename ...Args>
        inline auto construct_default_collection(const QJSValue& value, Args ... args) -> QJSValue {
            auto prototype = value.prototype();
            auto res = prototype.property("constructor").callAsConstructor();
            if (!res.isArray() && sizeof...(args) > 0) {
                auto next = construct_default_collection(args...);
                copy_if_same_type(res, next);
            }
            return res;
        }
#endif
        /* END key_value_xxx */

        template <typename T>
        inline bool p_isForInAble_(const T&) {
            return key_value_info<T>::is_key_value_type;
        }

#ifdef QT_QUICK_LIB
        inline bool p_isForInAble_(const QJSValue& v) {
            return v.isObject();
        }

        template <typename T>
        inline auto p_isCollection_(const T& v) -> typename std::enable_if<is_qjsvalue<T>::value, bool>::type {
            return v.isArray();
        }
#endif

        template <typename T, typename Key>
        struct is_kyt_key_matched {
            using Info = key_value_info<T>;
            enum {
                value = Info::is_key_value_type &&
                        std::is_convertible<Key, typename Info::key_type>::value
            };
        };

        template <typename T, typename Key, typename Value>
        struct is_kyt_key_value_matched {
            using Info = key_value_info<T>;
            enum {
                value = Info::is_key_value_type &&
                        std::is_convertible<Key, typename Info::key_type>::value &&
                        std::is_convertible<Value, typename Info::value_type>::value
            };
        };

        template <typename Meta,typename Key, typename Value>
        struct is_kyt_key_value_custom_matched {
            using Info = key_value_info<Meta>;
            enum {
                value = Info::is_key_value_type &&
                is_custom_convertible<Key, typename Info::key_type>::value &&
                is_custom_convertible<Value, typename Info::value_type>::value
            };
        };

        template <typename T, typename Key, typename Value, typename Ret>
        using enable_if_is_kyt_type_key_value_matched_ret = typename std::enable_if<is_kyt_key_value_matched<T, Key, Value>::value, Ret>;

        template <typename Meta, typename Key, typename Value, typename Ret>
        using enable_if_is_kyt_key_value_only_custom_matched_ret = typename std::enable_if<!is_kyt_key_value_matched<Meta, Key, Value>::value && is_kyt_key_value_custom_matched<Meta, Key, Value>::value, Ret>;

        template <typename T>
        using map_key_type_t = typename key_value_info<T>::key_type;

        template <typename T>
        using map_mapped_type_t = typename key_value_info<T>::value_type;

        template <typename Key, typename Value>
        inline Value map_iterator_value(typename std::map<Key,Value>::const_iterator & iter) {
            return iter->second;
        }

        template <typename Key, typename Value>
        inline Value map_iterator_value(typename std::map<Key,Value>::iterator & iter) {
            return iter->second;
        }

        template <typename Key, typename Value>
        inline Value map_iterator_value(typename QMap<Key,Value>::const_iterator & iter) {
            return iter.value();
        }

        template <typename Key, typename Value>
        inline Value map_iterator_value(typename QMap<Key,Value>::iterator & iter) {
            return iter.value();
        }

        template <typename Key, typename Value>
        inline Key map_iterator_key(typename std::map<Key,Value>::const_iterator & iter) {
            return iter->first;
        }

        template <typename Key, typename Value>
        inline Key map_iterator_key(typename std::map<Key,Value>::iterator & iter) {
            return iter->first;
        }

        template <typename Key, typename Value>
        inline Key map_iterator_key(typename QMap<Key,Value>::const_iterator & iter) {
            return iter.key();
        }

        template <typename Key, typename Value>
        inline Key map_iterator_key(typename QMap<Key,Value>::iterator & iter) {
            return iter.key();
        }

        template <typename Map>
        inline auto key_value_read(const Map& map, const map_key_type_t<Map> &key) ->
            typename std::enable_if<is_map<Map>::value, map_mapped_type_t<Map>>::type {
            map_mapped_type_t<Map> ret = map_mapped_type_t<Map>();
            auto iter = map.find(key);
            if (iter != map.end()) {
                ret = map_iterator_value<map_key_type_t<Map>, map_mapped_type_t<Map> >(iter);
            }
            return ret;
        }

        template <typename Map, typename Key, typename Value>
        inline auto key_value_write(Map& map, const Key& key, const Value& value) ->
        typename std::enable_if<is_map<Map>::value, bool>::type {
            map[key] = value;
            return true;
        }

        template <typename T, typename Ret>
        using enable_if_is_map = typename std::enable_if<is_map<T>::value, Ret>::type;

        template <typename T>
        using enable_if_is_map_ret_mapped_type = typename std::enable_if<is_map<T>::value, map_mapped_type_t<T>>;

        template <typename T, typename Key>
        using enable_if_is_kyt_key_matched = typename std::enable_if<is_kyt_key_matched<T,Key>::value, map_mapped_type_t<T>>;

        template <typename T>
        using enable_if_is_collection_ret_value_type = typename std::enable_if< is_static_collection<typename std::remove_reference<T>::type>::value, typename std::remove_reference<T>::type::value_type>;

        template <typename Meta>
        struct is_meta_object: std::integral_constant<int, key_value_info<Meta>::is_key_value_type> {};

        template <typename T>
        struct is_key_value_type: std::integral_constant<int, key_value_info<T>::is_key_value_type> {};

        template <typename T>
        struct is_real_key_value_type: std::integral_constant<int,key_value_info<T>::is_key_value_type && !std::is_same<T,QJSValue>::value> {};

        template <typename T>
        struct is_static_qt_metable: std::integral_constant<int,key_value_info<T>::is_key_value_type && !is_std_map<T>::value> {};

        template <typename T>
        struct is_static_qt_metable_castable: std::integral_constant<int,is_static_qt_metable<T>::value || std::is_same<T,QVariant>::value> {};

        template <typename T>
        struct is_static_real_qt_metable: std::integral_constant<int,is_real_key_value_type<T>::value && !is_std_map<T>::value> {};

        template <typename T, typename F1, typename F2, typename F3>
        inline bool try_cast_to_real_qt_metable(T& value, F1 func1, F2 func2, F3 func3) {
            auto qobject = cast_to_qobject(value);
            if (qobject) {
                func1(qobject);
                return true;
            }
            GadgetContainer gadget = cast_to_gadget_container(value);
            if (gadget.metaObject != nullptr) {
                func2(gadget);
                return true;
            }
            if (can_cast_to_qvariantmap(value)) {
                auto map = cast_to_qvariantmap(value);
                func3(map);
                return true;
            }
            return false;
        }

        template <typename T, typename F1, typename F2, typename F3, typename F4>
        inline bool try_cast_to_qt_metable(T& value, F1 func1, F2 func2, F3 func3, F4 func4) {
            if (is_qjsvalue<T>::value && can_cast_to_qvariantmap(value)) {
                QJSValue jsValue = cast_to_qjsvalue(value);
                func4(jsValue);
                return true;
            }
            auto qobject = cast_to_qobject(value);
            if (qobject) {
                func1(qobject);
                return true;
            }
            GadgetContainer gadget = cast_to_gadget_container(value);
            if (gadget.metaObject != nullptr) {
                func2(gadget);
                return true;
            }
            if (can_cast_to_qvariantmap(value)) {
                auto map = cast_to_qvariantmap(value);
                func3(map);
                return true;
            }
            return false;
        }

        template <typename T>
        inline bool can_cast_to_qt_metable(T& value) {
            return try_cast_to_qt_metable(value, [&](QObject*) {
            }, [&](GadgetContainer& ){
            }, [&](QVariantMap&) {
            }, [&](QJSValue&) {
            });
        }

        template <typename T, typename K>
        inline auto contains(const T&, const K& ) -> typename std::enable_if<!is_key_value_type<T>::value, bool>::type { return false;}

        template <typename T, typename K>
        inline auto contains(const T& map, const K& key) -> typename std::enable_if<is_map<T>::value, bool>::type {
            auto iter = map.find(key);
            return iter != map.end();
        }

#ifdef QT_CORE_LIB
        template <typename T, typename K>
        inline auto contains(const T& object, const K& key) -> typename std::enable_if<is_qobject<T>::value, bool>::type {
            if (object == nullptr) return false;
            const QMetaObject* meta = object->metaObject();
            auto k = cast_to_const_char_container(key);
            int index = meta->indexOfProperty(k.data);
            if (index >= 0) return true;

            auto dynamicProperties = object->dynamicPropertyNames();
            return dynamicProperties.contains(key);
        }

        template <typename T, typename K>
        inline auto contains(const T&, const K& key) -> typename std::enable_if<is_gadget<T>::value, bool>::type {
            const QMetaObject meta = T::staticMetaObject;
            auto k = cast_to_const_char_container(key);
            return meta.indexOfProperty(k.data) >= 0;
        }

        template <typename T, typename K>
        inline auto contains(const T& object, const K& key) -> typename std::enable_if<std::is_same<T, GadgetContainer>::value, bool>::type {
            const QMetaObject* meta = object.metaObject;
            if (meta == nullptr) {
                return false;
            }
            auto k = cast_to_const_char_container(key);
            return meta->indexOfProperty(k.data) >= 0;
        }

#endif

        /// Source: https://stackoverflow.com/questions/5052211/changing-value-type-of-a-given-stl-container
        template <class Container, class NewType>
        struct rebind_array_value_type {
            using type = NullArray<Undefined>;
        };

        template <class ValueType, class... Args, template <class...> class Container, class NewType>
        struct rebind_array_value_type<Container<ValueType, Args...>, NewType>
        {
            using type = Container<NewType, typename rebind_array_value_type<Args, NewType>::type...>;
        };

        template <class Collection, class NewKeyType, class NewValueType>
        struct convert_collection_to_map {
            using type = NullMap<NewKeyType, NewValueType>;
        };

        /// Check is the Functor be able to take Args as input. It works with generic lambda.
        template <typename Functor,typename ...Args>
        struct is_args_compatible: std::integral_constant<int,
            std::is_convertible<Functor, std::function<void(Args...)> >::value
        >{};

        template <typename Type, typename Functor, typename ...Args>
        using enable_if_args_compatible = typename std::enable_if<is_args_compatible<Functor, Args &&...>::value, Type>;

        template <typename Type, typename Functor, typename ...Args>
        using enable_if_args_not_compatible = typename std::enable_if<!is_args_compatible<Functor, Args &&...>::value, Type>;

        /// Convert to a function with zero paramter. Only the return type remained the same.
        template <typename Functor>
        typename enable_if_args_compatible<decltype(std::declval<Functor>()()), Functor>::type
        decl_func0();

        template <typename Functor>
        typename enable_if_args_not_compatible<void, Functor>::type
        decl_func0();

        template <typename Functor, typename Arg1>
        typename enable_if_args_compatible<decltype(std::declval<Functor>()(std::declval<Arg1>())), Functor, Arg1>::type
        decl_func0();

        template <typename Functor, typename Arg1>
        typename enable_if_args_not_compatible<void, Functor, Arg1>::type
        decl_func0();

        template <typename Functor, typename Arg1, typename Arg2>
        typename enable_if_args_compatible<decltype(std::declval<Functor>()(std::declval<Arg1>(), std::declval<Arg2>())), Functor, Arg1, Arg2>::type
        decl_func0();

        template <typename Functor, typename Arg1, typename Arg2>
        typename enable_if_args_not_compatible<void, Functor, Arg1, Arg2>::type
        decl_func0();

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3>
        typename enable_if_args_compatible<decltype(std::declval<Functor>()(std::declval<Arg1>(), std::declval<Arg2>(), std::declval<Arg3>())), Functor, Arg1, Arg2, Arg3>::type
        decl_func0();

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3>
        typename enable_if_args_not_compatible<void, Functor, Arg1, Arg2, Arg3>::type
        decl_func0();

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
        typename enable_if_args_compatible<decltype(std::declval<Functor>()(std::declval<Arg1>(), std::declval<Arg2>(), std::declval<Arg3>(), std::declval<Arg4>())), Functor, Arg1, Arg2, Arg3, Arg4>::type
        decl_func0();

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
        typename enable_if_args_not_compatible<void, Functor, Arg1, Arg2, Arg3, Arg4>::type
        decl_func0();

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
        typename enable_if_args_compatible<decltype(std::declval<Functor>()(std::declval<Arg1>(), std::declval<Arg2>(), std::declval<Arg3>(), std::declval<Arg4>(), std::declval<Arg5>())), Functor, Arg1, Arg2, Arg3, Arg4, Arg5>::type
        decl_func0();

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
        typename enable_if_args_not_compatible<void, Functor, Arg1, Arg2, Arg3, Arg4, Arg5>::type
        decl_func0();

        template <typename Functor, typename ...Args>
        struct ret_func {
            using type = decltype(decl_func0<Functor, Args&&...>());
        };

        /* BEGIN Collection */

        template <typename T, typename ...Args>
        class _collection_info {
        public:
            enum { is_collection_type = false };
            typedef Undefined size_type;
            typedef Undefined value_type;

            template <typename Any>
            static unsigned int size(const Any&) {return 0;}
            static Undefined getValue(const T&, unsigned int) { return Undefined(); }
            static void setValue(T&, unsigned int, const value_type&) {}

            template <typename Any>
            static void appendDefaultObject(Any&){}
        };

        template <typename T>
        class _collection_info<T, typename std::enable_if<is_static_collection<remove_cvref_t<T>>::value, std::true_type>::type> {
        public:
            enum { is_collection_type = true };
            using size_type = typename remove_cvref_t<T>::size_type;
            using value_type = typename remove_cvref_t<T>::value_type;

            static unsigned int size(const T& t) {return static_cast<unsigned int>(t.size());}
            static value_type getValue(const T& t, unsigned int index) { return t[index]; }
            static void setValue(T& t, unsigned int index, const value_type& value) { t[index] = value;}
            static void appendDefaultObject(T& t) { t.push_back(value_type()); }
        };

        template <typename T>
        class _collection_info<T, typename std::enable_if<is_qvariant<T>::value, std::true_type>::type> : public _collection_info<Undefined,Undefined> {
        public:
            enum { is_collection_type = true };
            using size_type = int;
            using value_type = QVariant;
#ifdef QT_CORE_LIB
            static unsigned int size(const T& t) {
                return t.type() == QVariant::List ? t.toList().size() : 0;
            }
            static value_type getValue(const T& t, unsigned int index) { return t.toList()[index]; }
            static void setValue(T& t, unsigned int index, const value_type& value) {
                if (t.type() != QVariant::List) {
                    return;
                }
                auto list = t.toList();
                list[index] = value;
                t = list;
            }
            static void appendDefaultObject(T& t){
                if (t.type() != QVariant::List) {
                    return;
                }
                auto list = t.toList();
                list.push_back(value_type());
                t = list;
            }
#endif
        };

        template <typename T>
        class _collection_info<T, typename std::enable_if<is_qjsvalue<T>::value, std::true_type>::type> : public _collection_info<Undefined,Undefined> {
        public:
            enum { is_collection_type = true };
            using size_type = int;
            using value_type = QJSValue;
#ifdef QT_QUICK_LIB
            static unsigned int size(const T& t) {
                return t.isArray() ? t.property("length").toInt() : 0;
            }
            static value_type getValue(const T& t, unsigned int index) {
                return t.property(index);
            }
            static void setValue(T& t, unsigned int index, const value_type& value) {
                t.setProperty(index, value);
            }
            static void appendDefaultObject(T& t) {
                auto value = contruct_default_object(t);
                QJSValueList args{value};
                t.property("push").callWithInstance(t, args);
            }
#else
            static QJSValue getValue(const T&, unsigned int) { return QJSValue(); }
            template <typename Any1, typename Any2>
            static void setValue(Any1&, unsigned int, const Any2&){}
#endif
        };

        template <typename T>
        class collection_info: public _collection_info<T, std::true_type> {};

        template <typename Array>
        struct collection_value_type {
            using type = typename collection_info<Array>::value_type;
        };

        template <typename Array>
        using collection_value_type_t = typename collection_info<Array>::value_type;

        template <typename Array>
        struct collection_size_type {
            using type = typename collection_info<Array>::size_type;
        };

        template <typename Collection, typename Index>
        struct is_collection_index_matched {
            enum {
                value = is_static_collection<Collection>::value && std::is_convertible<Index, int>::value
            };
        };

        template <typename Collection, typename Index>
        using enable_if_collection_index_matched = std::enable_if<is_collection_index_matched<Collection, Index>::value, typename collection_value_type<Collection>::type>;

        template <typename T>
        inline auto cast_to_collection(const T&) -> std::vector<Undefined> { return std::vector<Undefined>{}; }

        template <typename T>
        inline auto p_isCollection_(const T&) -> typename std::enable_if<!std::is_same<T,QVariant>::value && !is_real_qjsvalue<T>::value, bool>::type {
            return is_static_collection<T>::value;
        }

#ifdef QT_CORE_LIB
        inline QVariantList cast_to_collection(const QVariant& t) { return t.toList();}

        template <typename T>
        inline auto p_isCollection_(const T& v) -> typename std::enable_if<std::is_same<T,QVariant>::value, bool>::type {
            return v.type() == QVariant::List || QtMetaObjectTable<QtMetaObject::List>::storage().contains(v.userType());
        }
#endif

#ifdef QT_QUICK_LIB
        inline QJSValue cast_to_collection(const QJSValue& t) { return t;}
#endif

        template <typename T> // Convert to collection but it may perform type conversion
        inline auto p_convertToCollection_(const T& object) -> decltype(cast_to_collection(object)) {
            return cast_to_collection(object);
        }

#ifdef QT_CORE_LIB
        inline auto p_convertToCollection_(const QVariant& object) -> QVariantList {
            if (object.type() == QVariant::List) {
                return object.toList();
            }

            auto table = QtMetaObjectTable<QtMetaObject::List>::storage();
            int type = object.userType();
            if (table.contains(type)) {
                return table[type].toVariantList(object);
            }
            return QVariantList();
        }

        template <typename Ref, typename ...Args>
        inline auto p_convertToCollection_or_construct(const Ref& ref, Args ...args) -> decltype(p_convertToCollection_(ref)) {
            auto ret = p_convertToCollection_(ref);
            if (!p_isCollection_(ref) && sizeof...(args) > 0) {
                auto next = construct_default_collection(ref, args...);
                copy_if_same_type(ret, next);
            }
            return ret;
        }
#endif

        /* END Collection */

        /// Read a property from the target container object
        template <typename Map, typename Key>
        inline auto read(const Map &map, Key key) ->
            typename enable_if_is_kyt_key_matched<Map, Key>::type {
            return key_value_read<Map>(map, key);
        }

        template <typename Collection, typename Index>
        inline auto read(const Collection& collection, Index index) -> typename enable_if_collection_index_matched<Collection, Index>::type {
            return collection[index];
        }

        template <typename Any, typename Key>
        struct is_readable {
            enum {
                value = is_kyt_key_matched<Any, Key>::value ||
                        is_collection_index_matched<Any, Key>::value
            };
        };

        template <typename Other, typename Key>
        inline auto read(const Other, Key) -> typename std::enable_if<!is_readable<Other,Key>::value,
            Undefined
        >::type {
            return Undefined();
        }

        template <typename Map, typename Key, typename Value>
        inline auto write(Map &map, const Key& key, const Value& value) ->
            typename enable_if_is_kyt_type_key_value_matched_ret<Map, Key, Value, bool>::type {
            key_value_write(map, key, value);
            return true;
        }

        template <typename Meta, typename Key, typename Value>
        inline auto write(Meta &map, const Key& key, const Value& value) ->
            typename enable_if_is_kyt_key_value_only_custom_matched_ret
<Meta, Key, Value, bool>::type { // eg. QJSValue to QVariant
            typename key_value_info<Meta>::key_type k;
            typename key_value_info<Meta>::value_type v;
            convertTo(key, k);
            convertTo(value, v);

            key_value_write(map, k, v);
            return true;
        }

        template <typename T, typename Key, typename Value>
        inline auto write(T&, Key&, Value&) -> typename std::enable_if<!is_kyt_key_value_matched<T,Key,Value>::value && !is_kyt_key_value_custom_matched<T,Key,Value>::value, bool>::type { return false;};

#ifdef QT_QUICK_LIB
        template <typename Key>
        inline auto write(QJSValue &, const Key&, const QVariant&) -> bool {
            return false;
        }
#endif

        template <typename Any, typename Key>
        struct ret_read {
            typedef decltype(read(std::declval<Any>(), std::declval<Key>())) type;
        };

        template <typename Object, typename Key, typename ...Args>
        inline auto invoke(const Key& key, const Object& object) -> typename std::enable_if<is_readable<Object, Key>::value, typename ret_read<Object,Key>::type>::type {
            return read(object, key);
        }

        /* BEGIN_GENERATED_CODE */
        template <typename Functor>
        inline auto invoke(Functor functor) -> 
        typename std::enable_if<is_args_compatible<Functor>::value,
        typename ret_func<Functor>::type>::type {
            return functor();
        }

        template <typename Functor>
        inline auto invoke(Functor) -> 
        typename std::enable_if<!is_args_compatible<Functor>::value,
        Undefined>::type {
            return Undefined();
        }

        template <typename Functor>
        struct is_invokable0 {
            enum {
               value = is_args_compatible<Functor>::value
            };
        };

        
        template <typename Functor>
        typename std::enable_if<is_invokable0<Functor>::value, decltype(invoke(std::declval<Functor>()))>::type
        inline decl_invoke0();
        
        template <typename Functor>
        typename std::enable_if<!is_invokable0<Functor>::value, Undefined>::type
        inline decl_invoke0();

        template <typename Functor, typename Arg1>
        inline auto invoke(Functor functor, Arg1) -> 
        typename std::enable_if<is_args_compatible<Functor>::value,
        typename ret_func<Functor>::type>::type {
            return functor();
        }

        template <typename Functor, typename Arg1>
        inline auto invoke(Functor functor, Arg1 arg1) -> 
        typename std::enable_if<is_args_compatible<Functor, Arg1>::value,
        typename ret_func<Functor, Arg1>::type>::type {
            return functor(arg1);
        }

        template <typename Functor, typename Arg1>
        inline auto invoke(Functor, Arg1) -> 
        typename std::enable_if<!is_args_compatible<Functor>::value && !is_args_compatible<Functor, Arg1>::value && !is_readable<Arg1, Functor>::value,
        Undefined>::type {
            return Undefined();
        }

        template <typename Functor, typename Arg1>
        struct is_invokable1 {
            enum {
               value = is_args_compatible<Functor>::value || is_args_compatible<Functor, Arg1>::value || is_readable<Arg1, Functor>::value
            };
        };

        
        template <typename Functor, typename Arg1>
        typename std::enable_if<is_invokable1<Functor, Arg1>::value, decltype(invoke(std::declval<Functor>(),std::declval<Arg1>()))>::type
        inline decl_invoke0();
        
        template <typename Functor, typename Arg1>
        typename std::enable_if<!is_invokable1<Functor, Arg1>::value, Undefined>::type
        inline decl_invoke0();

        template <typename Functor, typename Arg1, typename Arg2>
        inline auto invoke(Functor functor, Arg1, Arg2) -> 
        typename std::enable_if<is_args_compatible<Functor>::value,
        typename ret_func<Functor>::type>::type {
            return functor();
        }

        template <typename Functor, typename Arg1, typename Arg2>
        inline auto invoke(Functor functor, Arg1 arg1, Arg2) -> 
        typename std::enable_if<is_args_compatible<Functor, Arg1>::value,
        typename ret_func<Functor, Arg1>::type>::type {
            return functor(arg1);
        }

        template <typename Functor, typename Arg1, typename Arg2>
        inline auto invoke(Functor functor, Arg1 arg1, Arg2 arg2) -> 
        typename std::enable_if<is_args_compatible<Functor, Arg1, Arg2>::value,
        typename ret_func<Functor, Arg1, Arg2>::type>::type {
            return functor(arg1, arg2);
        }

        template <typename Functor, typename Arg1, typename Arg2>
        inline auto invoke(Functor, Arg1, Arg2) -> 
        typename std::enable_if<!is_args_compatible<Functor>::value && !is_args_compatible<Functor, Arg1>::value && !is_args_compatible<Functor, Arg1, Arg2>::value && !is_readable<Arg1, Functor>::value,
        Undefined>::type {
            return Undefined();
        }

        template <typename Functor, typename Arg1, typename Arg2>
        struct is_invokable2 {
            enum {
               value = is_args_compatible<Functor>::value || is_args_compatible<Functor, Arg1>::value || is_args_compatible<Functor, Arg1, Arg2>::value || is_readable<Arg1, Functor>::value
            };
        };

        
        template <typename Functor, typename Arg1, typename Arg2>
        typename std::enable_if<is_invokable2<Functor, Arg1, Arg2>::value, decltype(invoke(std::declval<Functor>(),std::declval<Arg1>(),std::declval<Arg2>()))>::type
        inline decl_invoke0();
        
        template <typename Functor, typename Arg1, typename Arg2>
        typename std::enable_if<!is_invokable2<Functor, Arg1, Arg2>::value, Undefined>::type
        inline decl_invoke0();

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3>
        inline auto invoke(Functor functor, Arg1, Arg2, Arg3) -> 
        typename std::enable_if<is_args_compatible<Functor>::value,
        typename ret_func<Functor>::type>::type {
            return functor();
        }

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3>
        inline auto invoke(Functor functor, Arg1 arg1, Arg2, Arg3) -> 
        typename std::enable_if<is_args_compatible<Functor, Arg1>::value,
        typename ret_func<Functor, Arg1>::type>::type {
            return functor(arg1);
        }

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3>
        inline auto invoke(Functor functor, Arg1 arg1, Arg2 arg2, Arg3) -> 
        typename std::enable_if<is_args_compatible<Functor, Arg1, Arg2>::value,
        typename ret_func<Functor, Arg1, Arg2>::type>::type {
            return functor(arg1, arg2);
        }

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3>
        inline auto invoke(Functor functor, Arg1 arg1, Arg2 arg2, Arg3 arg3) -> 
        typename std::enable_if<is_args_compatible<Functor, Arg1, Arg2, Arg3>::value,
        typename ret_func<Functor, Arg1, Arg2, Arg3>::type>::type {
            return functor(arg1, arg2, arg3);
        }

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3>
        inline auto invoke(Functor, Arg1, Arg2, Arg3) -> 
        typename std::enable_if<!is_args_compatible<Functor>::value && !is_args_compatible<Functor, Arg1>::value && !is_args_compatible<Functor, Arg1, Arg2>::value && !is_args_compatible<Functor, Arg1, Arg2, Arg3>::value && !is_readable<Arg1, Functor>::value,
        Undefined>::type {
            return Undefined();
        }

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3>
        struct is_invokable3 {
            enum {
               value = is_args_compatible<Functor>::value || is_args_compatible<Functor, Arg1>::value || is_args_compatible<Functor, Arg1, Arg2>::value || is_args_compatible<Functor, Arg1, Arg2, Arg3>::value || is_readable<Arg1, Functor>::value
            };
        };

        
        template <typename Functor, typename Arg1, typename Arg2, typename Arg3>
        typename std::enable_if<is_invokable3<Functor, Arg1, Arg2, Arg3>::value, decltype(invoke(std::declval<Functor>(),std::declval<Arg1>(),std::declval<Arg2>(),std::declval<Arg3>()))>::type
        inline decl_invoke0();
        
        template <typename Functor, typename Arg1, typename Arg2, typename Arg3>
        typename std::enable_if<!is_invokable3<Functor, Arg1, Arg2, Arg3>::value, Undefined>::type
        inline decl_invoke0();

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
        inline auto invoke(Functor functor, Arg1, Arg2, Arg3, Arg4) -> 
        typename std::enable_if<is_args_compatible<Functor>::value,
        typename ret_func<Functor>::type>::type {
            return functor();
        }

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
        inline auto invoke(Functor functor, Arg1 arg1, Arg2, Arg3, Arg4) -> 
        typename std::enable_if<is_args_compatible<Functor, Arg1>::value,
        typename ret_func<Functor, Arg1>::type>::type {
            return functor(arg1);
        }

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
        inline auto invoke(Functor functor, Arg1 arg1, Arg2 arg2, Arg3, Arg4) -> 
        typename std::enable_if<is_args_compatible<Functor, Arg1, Arg2>::value,
        typename ret_func<Functor, Arg1, Arg2>::type>::type {
            return functor(arg1, arg2);
        }

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
        inline auto invoke(Functor functor, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4) -> 
        typename std::enable_if<is_args_compatible<Functor, Arg1, Arg2, Arg3>::value,
        typename ret_func<Functor, Arg1, Arg2, Arg3>::type>::type {
            return functor(arg1, arg2, arg3);
        }

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
        inline auto invoke(Functor functor, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) -> 
        typename std::enable_if<is_args_compatible<Functor, Arg1, Arg2, Arg3, Arg4>::value,
        typename ret_func<Functor, Arg1, Arg2, Arg3, Arg4>::type>::type {
            return functor(arg1, arg2, arg3, arg4);
        }

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
        inline auto invoke(Functor, Arg1, Arg2, Arg3, Arg4) -> 
        typename std::enable_if<!is_args_compatible<Functor>::value && !is_args_compatible<Functor, Arg1>::value && !is_args_compatible<Functor, Arg1, Arg2>::value && !is_args_compatible<Functor, Arg1, Arg2, Arg3>::value && !is_args_compatible<Functor, Arg1, Arg2, Arg3, Arg4>::value && !is_readable<Arg1, Functor>::value,
        Undefined>::type {
            return Undefined();
        }

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
        struct is_invokable4 {
            enum {
               value = is_args_compatible<Functor>::value || is_args_compatible<Functor, Arg1>::value || is_args_compatible<Functor, Arg1, Arg2>::value || is_args_compatible<Functor, Arg1, Arg2, Arg3>::value || is_args_compatible<Functor, Arg1, Arg2, Arg3, Arg4>::value || is_readable<Arg1, Functor>::value
            };
        };

        
        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
        typename std::enable_if<is_invokable4<Functor, Arg1, Arg2, Arg3, Arg4>::value, decltype(invoke(std::declval<Functor>(),std::declval<Arg1>(),std::declval<Arg2>(),std::declval<Arg3>(),std::declval<Arg4>()))>::type
        inline decl_invoke0();
        
        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
        typename std::enable_if<!is_invokable4<Functor, Arg1, Arg2, Arg3, Arg4>::value, Undefined>::type
        inline decl_invoke0();

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
        inline auto invoke(Functor functor, Arg1, Arg2, Arg3, Arg4, Arg5) -> 
        typename std::enable_if<is_args_compatible<Functor>::value,
        typename ret_func<Functor>::type>::type {
            return functor();
        }

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
        inline auto invoke(Functor functor, Arg1 arg1, Arg2, Arg3, Arg4, Arg5) -> 
        typename std::enable_if<is_args_compatible<Functor, Arg1>::value,
        typename ret_func<Functor, Arg1>::type>::type {
            return functor(arg1);
        }

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
        inline auto invoke(Functor functor, Arg1 arg1, Arg2 arg2, Arg3, Arg4, Arg5) -> 
        typename std::enable_if<is_args_compatible<Functor, Arg1, Arg2>::value,
        typename ret_func<Functor, Arg1, Arg2>::type>::type {
            return functor(arg1, arg2);
        }

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
        inline auto invoke(Functor functor, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4, Arg5) -> 
        typename std::enable_if<is_args_compatible<Functor, Arg1, Arg2, Arg3>::value,
        typename ret_func<Functor, Arg1, Arg2, Arg3>::type>::type {
            return functor(arg1, arg2, arg3);
        }

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
        inline auto invoke(Functor functor, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5) -> 
        typename std::enable_if<is_args_compatible<Functor, Arg1, Arg2, Arg3, Arg4>::value,
        typename ret_func<Functor, Arg1, Arg2, Arg3, Arg4>::type>::type {
            return functor(arg1, arg2, arg3, arg4);
        }

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
        inline auto invoke(Functor functor, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5) -> 
        typename std::enable_if<is_args_compatible<Functor, Arg1, Arg2, Arg3, Arg4, Arg5>::value,
        typename ret_func<Functor, Arg1, Arg2, Arg3, Arg4, Arg5>::type>::type {
            return functor(arg1, arg2, arg3, arg4, arg5);
        }

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
        inline auto invoke(Functor, Arg1, Arg2, Arg3, Arg4, Arg5) -> 
        typename std::enable_if<!is_args_compatible<Functor>::value && !is_args_compatible<Functor, Arg1>::value && !is_args_compatible<Functor, Arg1, Arg2>::value && !is_args_compatible<Functor, Arg1, Arg2, Arg3>::value && !is_args_compatible<Functor, Arg1, Arg2, Arg3, Arg4>::value && !is_args_compatible<Functor, Arg1, Arg2, Arg3, Arg4, Arg5>::value && !is_readable<Arg1, Functor>::value,
        Undefined>::type {
            return Undefined();
        }

        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
        struct is_invokable5 {
            enum {
               value = is_args_compatible<Functor>::value || is_args_compatible<Functor, Arg1>::value || is_args_compatible<Functor, Arg1, Arg2>::value || is_args_compatible<Functor, Arg1, Arg2, Arg3>::value || is_args_compatible<Functor, Arg1, Arg2, Arg3, Arg4>::value || is_args_compatible<Functor, Arg1, Arg2, Arg3, Arg4, Arg5>::value || is_readable<Arg1, Functor>::value
            };
        };

        
        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
        typename std::enable_if<is_invokable5<Functor, Arg1, Arg2, Arg3, Arg4, Arg5>::value, decltype(invoke(std::declval<Functor>(),std::declval<Arg1>(),std::declval<Arg2>(),std::declval<Arg3>(),std::declval<Arg4>(),std::declval<Arg5>()))>::type
        inline decl_invoke0();
        
        template <typename Functor, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
        typename std::enable_if<!is_invokable5<Functor, Arg1, Arg2, Arg3, Arg4, Arg5>::value, Undefined>::type
        inline decl_invoke0();

        /* END_GENERATED_CODE */

        template <typename Functor, typename ...Args>
        struct ret_invoke {
            using type = decltype(decl_invoke0<Functor, Args...>());
        };

        template <typename Functor, typename ...Args>
        struct ret_invoke_is_not_void {
            enum{
                value = !std::is_same<void, typename ret_invoke<Functor, Args...>::type>::value
            };
        };

        template <typename Iteratee, typename Collection>
        using ret_invoke_collection_value_type_t = typename ret_invoke<Iteratee, typename collection_value_type<Collection>::type>::type;

        template <class Collection, typename Iteratee, typename ValueType>
        using RebindedMap_keyIterateeRet = typename convert_collection_to_map<remove_cvref_t<Collection>,   _::Private::ret_invoke_collection_value_type_t<Iteratee,remove_cvref_t<Collection>>, ValueType>::type;

        ///Value is a wrapper of any data structure include <void>.
        template <typename T>
        class Value {
        public:
            template <typename Functor, typename ...Args>
            inline void invoke(Functor functor, Args... args) {
                value = _::Private::invoke(functor, args...);
            }

            template <typename Other>
            inline bool canConvert() {
                return std::is_convertible<T, Other>::value;
            }

            template <typename Any>
            inline typename std::enable_if<std::is_convertible<Any, T>::value, bool>::type
            equals(Any&& other) {
                return static_cast<T>(other) == value;
            }

            template <typename Any>
            inline typename std::enable_if<!std::is_convertible<Any, T>::value, bool>::type
            equals(Any&&) {
                return false;
            }

            inline T get() {
                return value;
            }

            T value;
        };

        template <>
        class Value<void> {
        public:
            template <typename Functor, typename ...Args>
            inline void invoke(Functor functor, Args... args) {
                _::Private::invoke(functor, args...);
            }

            template <typename Other>
            inline bool canConvert() {
                return false;
            }

            template <typename Any>
            inline bool equals(const Any&) {
                return false;
            }

            inline Undefined get() {
                return Undefined();
            }
        };

        /// via_func( VIA = Value,Index,Array);
        template <typename Functor, typename Array>
        struct via_func_info {

            using value_type = typename collection_value_type<Array>::type;
            using size_type = typename collection_size_type<Array>::type;

            using ret_type = typename ret_invoke<Functor, value_type, size_type, Array>::type;

            using non_void_ret_type = typename std::conditional<std::is_same<void, ret_type>::value, Undefined, ret_type>::type;

            enum {
                is_invokable = is_invokable3<Functor, value_type, size_type, Array>::value,
                is_void_ret = std::is_same<typename ret_invoke<Functor,value_type,size_type,Array>::type, void>::value
            };

        };

        template <typename V1, typename V2, typename K>
        inline auto write_if_same_type(V1 &v1, const K& k, const V2& v2) -> typename std::enable_if<std::is_same<V1,V2>::value, void>::type {
            write(v1, k, v2);
        }

        template <typename V1, typename V2, typename K>
        inline auto write_if_same_type(V1 &, const K&, const V2&) -> typename std::enable_if<!std::is_same<V1,V2>::value, void>::type {
        }

#ifdef QT_CORE_LIB
        template <class NewType>
        struct rebind_array_value_type<QStringList, NewType> {
            typedef QList<NewType> type;
        };

        template <class NewType>
        struct rebind_array_value_type<QVariantList, NewType> {
            typedef QList<NewType> type;
        };

        template <typename KeyValueType>
        inline auto p_recursive_get_(const KeyValueType& object, const QStringList& tokens ,int index , QVariant& result) -> void {
            auto k = cast_to_const_char_container(tokens[index]);
            QVariant value;
            bool hasKey = false;

            try_cast_to_real_qt_metable(object, [&](const QObject* kyt){
                if (contains(kyt, k.data)) { value = read(kyt, k.data); hasKey = true;}
            },[&](GadgetContainer& kyt) {
                if (contains(kyt, k.data)) { value = read(kyt, k.data); hasKey = true;}
            },[&](QVariantMap& kyt) {
                if (contains(kyt, k.data)) { value = read(kyt, k.data); hasKey = true;}
            });

            int remaining = static_cast<int>(tokens.size()) - index - 1;
            if (remaining == 0 && hasKey) {
                result = value;
            }

            if (remaining != 0 && hasKey) {
                p_recursive_get_(value, tokens, index + 1, result);
            }
        }

        template <typename KeyValueType>
        inline QVariant p_get_(const KeyValueType& object, const QString& path, const QVariant& defaultValue) {
            QVariant result = defaultValue;
            auto tokens = path.split(".");
            p_recursive_get_(object, tokens, 0, result);
            return result;
        }

        template <typename KeyValueType, typename QtAny>
        inline pointer_or_reference_t<KeyValueType> p_recursive_set_(KeyValueType& object, const QStringList& tokens,int index, const QtAny& value) {
            auto k = cast_to_const_char_container(tokens[index]);

            int remaining = static_cast<int>(tokens.size()) - index - 1;

            if (remaining == 0) {

                try_cast_to_qt_metable(object, [&](QObject* kyt){
                    write(kyt, k.data, value);
                },[&](GadgetContainer& kyt) {
                    write(kyt, k.data, value);
                },[&](QVariantMap& kyt) {
                    write(kyt, k.data, value);
                    copy_if_same_type(object, kyt);
                },[&](QJSValue &kyt) {
                    write(kyt, k.data, value);
                    copy_if_same_type(object, kyt);
                });

            } else {
                try_cast_to_qt_metable(object, [&](const QObject* kyt){
                    if (contains(kyt, k.data)) {
                        auto v = read(kyt, k.data); p_recursive_set_(v , tokens, index + 1, value);
                    }
                },[&](GadgetContainer& kyt) {
                    if (contains(kyt, k.data)) {
                        auto v = read(kyt, k.data); p_recursive_set_(v , tokens, index + 1, value);
                    }
                },[&](QVariantMap& kyt) {
                    auto v = read(kyt, k.data).toMap();
                    p_recursive_set_(v , tokens, index + 1, value);
                    write_if_same_type(object, k.data, v);
                },[&](QJSValue &kyt) {
                    auto v = read(kyt, k.data);
                    if (v.isUndefined()) {
                        copy_if_same_type(v, contruct_default_object(kyt));
                    }
                    p_recursive_set_(v , tokens, index + 1, value);
                    write_if_same_type(object, k.data, v);
                });
            }

            return object;
        }

        template <typename KeyValueType, typename QtAny>
        inline void p_set_(KeyValueType& object, const QString& path, const QtAny& value) {
            auto tokens = path.split(".");
            p_recursive_set_(object, tokens, 0, value);
        }
#endif

        /* BEGIN p_forIn_ */

        template <typename Map, typename Functor>
        inline auto p_forIn_(const Map& object, Functor iteratee) -> typename std::enable_if<Private::is_map<Map>::value, const Map&>::type {
            using K = typename Private::key_value_info<Map>::key_type;
            using V = typename Private::key_value_info<Map>::value_type;

            static_assert(Private::is_invokable3<Functor, V, K, Map>::value, "_::forIn: " _underline_iteratee_mismatched_error);

            Private::Value<typename Private::ret_invoke<Functor, V, K, Map>::type> value;

            auto iter = object.begin();
            while (iter != object.end()) {
                value.invoke(iteratee, Private::map_iterator_value<K,V>(iter),
                                       Private::map_iterator_key<K,V>(iter),
                                       object);

                if (value.template canConvert<bool>() && value.equals(false)) {
                    break;
                }
                iter++;
            }

            return object;
        }

#ifdef QT_CORE_LIB
        template <typename Object, typename Functor>
        inline auto p_forIn_(const Object* object, Functor iteratee) -> typename std::enable_if<Private::is_qobject<Object>::value, const Object*>::type {
            if (object == nullptr) {
                return object;
            }
            const QMetaObject* meta = object->metaObject();
            static_assert(Private::is_invokable3<Functor, QVariant, QString, QObject*>::value, "_::forIn: " _underline_iteratee_mismatched_error);
            Private::Value<typename Private::ret_invoke<Functor, QVariant, QString, QObject*>::type> invokeHelper;

            for (int i = 0 ; i < meta->propertyCount(); i++) {
                const QMetaProperty property = meta->property(i);
                QString key = property.name();
                QVariant value = property.read(object);

                invokeHelper.invoke(iteratee, value, key, object);

                if (invokeHelper.template canConvert<bool>() && invokeHelper.equals(false)) {
                    return object;
                }
            }

            QList<QByteArray> dynamicProperties = object->dynamicPropertyNames();
            for (auto p : dynamicProperties) {
                QVariant value = object->property(p);
                invokeHelper.invoke(iteratee, value, QString(p), object);
                if (invokeHelper.template canConvert<bool>() && invokeHelper.equals(false)) {
                    return object;
                }
            }

            return object;
        }

        template <typename Object, typename Functor>
        inline auto p_forIn_(const Object& object, Functor iteratee) -> typename std::enable_if<Private::is_gadget<Object>::value, const Object&>::type {
            auto ptr = cast_to_pointer(object);

            const QMetaObject meta = ptr->staticMetaObject;

            static_assert(Private::is_invokable3<Functor, QVariant, QString, const Object&>::value, "_::forIn: " _underline_iteratee_mismatched_error);
            Private::Value<typename Private::ret_invoke<Functor, QVariant, QString, const Object&>::type> invokeHelper;

            for (int i = 0 ; i < meta.propertyCount(); i++) {
                const QMetaProperty property = meta.property(i);
                QString key = property.name();
                QVariant value = property.readOnGadget(ptr);

                invokeHelper.invoke(iteratee, value, key, object);

                if (invokeHelper.template canConvert<bool>() && invokeHelper.equals(false)) {
                    break;
                }
            }
            return object;
        }

        template <typename Object, typename Functor>
        inline auto p_forIn_(const Object& object, Functor iteratee) -> typename std::enable_if<std::is_same<Object, GadgetContainer>::value, const Object&>::type {
            if (object.constData == nullptr) {
                return object;
            }

            const QMetaObject* meta = object.metaObject;
            Private::Value<typename Private::ret_invoke<Functor, QVariant, QString, const Object&>::type> invokeHelper;

            for (int i = 0 ; i < meta->propertyCount(); i++) {
                const QMetaProperty property = meta->property(i);
                QString key = property.name();
                QVariant value = property.readOnGadget(object.constData);

                invokeHelper.invoke(iteratee, value, key, object);

                if (invokeHelper.template canConvert<bool>() && invokeHelper.equals(false)) {
                    break;
                }
            }
            return object;
        }
#endif

#ifdef QT_QUICK_LIB
        template <typename Value, typename Iteratee>
        inline auto p_forIn_(const Value& object, Iteratee iteratee)
        -> typename std::enable_if<std::is_same<Value,QJSValue>::value, const Value&>::type
        {
            QJSValueIterator iter(object);
            Private::Value<typename Private::ret_invoke<Iteratee, Value, QString, Value>::type> value;

            while (iter.hasNext()) {
                iter.next();
                value.invoke(iteratee, iter.value(), iter.name(), object);

                if (value.template canConvert<bool>() && value.equals(false)) {
                    break;
                }
            }

            return object;
        }
#else
        template <typename Value, typename Iteratee>
        inline auto p_forIn_(const Value& object, Iteratee)
        -> typename std::enable_if<std::is_same<Value,QJSValue>::value, const Value&>::type
        {
            return object;
        }
#endif
#ifdef QT_CORE_LIB
        template <typename Object, typename Iteratee>
        inline auto p_forIn_(const Object& object, Iteratee iteratee)
        -> typename std::enable_if<is_qvariant<Object>::value, const Object&>::type
        {
            try_cast_to_qt_metable(object, [&](QObject* metable){
                p_forIn_(metable, iteratee);
            },[&](GadgetContainer metable) {
                p_forIn_(metable, iteratee);
            },[&](QVariantMap& metable) {
                p_forIn_(metable, iteratee);
            },[&](QJSValue& metable) {
                p_forIn_(metable, iteratee);
            });
            return object;
        }
#endif
        template <typename Array, typename Iteratee>
        inline auto p_forEach_(Array, Iteratee) -> typename std::enable_if<!is_static_collection<Array>::value && !std::is_same<Array, QVariant>::value && !is_real_qjsvalue<Array>::value, void>::type {}

        template <typename Array, typename Iteratee>
        inline auto p_forEach_(Array& collection, Iteratee iteratee) -> typename std::enable_if<is_static_collection<Array>::value, Array&>::type {
            Private::Value<typename Private::ret_invoke<Iteratee, typename Private::collection_value_type<Array>::type, int, Array >::type> value;

            for (unsigned int i = 0 ; i < collection_info<Array>::size(collection) ; i++) {
                value.invoke(iteratee, collection[i], i, collection);
                if (value.template canConvert<bool>() && value.equals(false)) {
                    break;
                }
            }
            return collection;
        }

#ifdef QT_CORE_LIB
        template <typename Array, typename Iteratee>
        inline auto p_forEach_(Array& object, Iteratee iteratee) -> typename std::enable_if<is_qvariant<Array>::value, Array&>::type {
            if (object.type() == QVariant::List) {
                auto list = object.toList();
                p_forEach_(list, iteratee);
            } else {
                auto table = QtMetaObjectTable<QtMetaObject::List>::storage();
                if (table.contains(object.userType())) {
                    auto metaObject = table[object.userType()];
                    Private::Value<typename Private::ret_invoke<Iteratee, QVariant, int, QVariant>::type> value;

                    for (unsigned int i = 0 ; i < metaObject.count(object) ; i++) {
                        value.invoke(iteratee, metaObject.get(object,i), i, object);
                        if (value.template canConvert<bool>() && value.equals(false)) {
                            break;
                        }
                    }
                }
            }
            return object;
        }
#endif

#ifdef QT_QUICK_LIB
        template <typename Array, typename Iteratee>
        inline auto p_forEach_(Array& object, Iteratee iteratee) -> typename std::enable_if<is_qjsvalue<Array>::value, Array&>::type {
            QJSValueIterator iter(object);
            Private::Value<typename Private::ret_invoke<Iteratee, QJSValue, int, QJSValue>::type> value;

            while (iter.hasNext()) {
                iter.next();
                bool isInt;
                int index = iter.name().toInt(&isInt);
                if (!isInt) {
                    continue;
                }

                value.invoke(iteratee, iter.value(), index, object);

                if (value.template canConvert<bool>() && value.equals(false)) {
                    break;
                }
            }

            return object;
        }
#endif

        /* PRIVATE_MERGE begin */
#ifdef QT_CORE_LIB
        class MergePredicateObject {
        public:
            bool predicate(const QString &) const { return true;}
            MergePredicateObject next(const QString&) const { return *this;};
        };

        template <typename V1, typename V2, typename PredicateObject>
        inline auto p_merge_(V1& v1, const V2& v2, const PredicateObject &) -> pointer_or_reference_t<V1>;

        template <typename V1, typename V2, typename PredicateObject>
        inline void p_merge_forEach(V1& v1, const V2&v2, const PredicateObject& predicate) {
            using Value = typename collection_info<V2>::value_type;

            p_forEach_(v2, [&](const Value & value, unsigned int index) {
                for (unsigned int i = collection_info<V1>::size(v1) ; i <= index;i++ ) {
                    collection_info<V1>::appendDefaultObject(v1);
                }
                auto v1Size = collection_info<V1>::size(v1);

                if (index < v1Size) {
                    auto obj = collection_info<V1>::getValue(v1, index);
                    p_merge_(obj, value, predicate); //@TODO optimization
                    collection_info<V1>::setValue(v1, index, obj);
                }
            });
        }

        template <typename V1, typename V2>
        struct p_merge_forIn_cond {
            enum { value = is_key_value_type<V1>::value && (is_key_value_type<V2>::value || is_qvariant<V2>::value)};
        };

        template <typename V1, typename V2, typename PredicateObject>
        inline auto p_merge_forIn_(V1& v1 , const V2& v2, const PredicateObject& predicate) -> typename std::enable_if<p_merge_forIn_cond<V1,V2>::value, void>::type {
            using Key = typename key_value_info<V2>::key_type;
            using Value = typename key_value_info<V2>::value_type;

            p_forIn_(v2, [&](const Value& value, const Key& key) {
                if (!predicate.predicate(key)) {
                    return;
                }
                auto next = predicate.next(key);

                auto dstValue = read(v1, key);

                QObject* v1_qobject_ptr = cast_to_qobject(dstValue);
                if (v1_qobject_ptr != nullptr) {
                    p_merge_(v1_qobject_ptr, value, next);
                    return;
                }

                if (p_isCollection_(value)) {
                    auto l1 = p_convertToCollection_or_construct(dstValue, value);
                    auto l2 = p_convertToCollection_(value);
                    p_merge_forEach(l1, l2, next);
                    write(v1, key, l1);
                    return;
                }

                bool missingObjectAtPath = !can_cast_to_qt_metable(dstValue) && can_cast_to_qt_metable(value);
                if (missingObjectAtPath) {
                    auto path = contruct_default_object(v1);
                    write(v1, key, p_merge_(path, value, next));
                } else {
                    write(v1, key, p_merge_(dstValue, value, next));
                }
            });
        }

        template <typename V1, typename V2, typename PredicateObject>
        inline auto p_merge_forIn_(V1& , const V2&, const PredicateObject&) -> typename std::enable_if<!p_merge_forIn_cond<V1,V2>::value, void>::type {
        }

        template <typename V1, typename V2, typename PredicateObject>
        inline auto p_merge_(V1& v1, const V2& v2, const PredicateObject& predicate) -> pointer_or_reference_t<V1> {

            bool isV2Castable = p_isForInAble_(v2) || can_cast_to_qt_metable(v2);

            if (isV2Castable) {
                if (p_isForInAble_(v1)) {
                    p_merge_forIn_(v1, v2, predicate);
                    return v1;
                }

                bool forInAbleByCastingV1 = try_cast_to_qt_metable(v1, [&](QObject* metable){
                    p_merge_(metable, v2, predicate);
                },[&](GadgetContainer metable) {
                    p_merge_(metable, v2, predicate);
                },[&](QVariantMap& metable) {
                    p_merge_(metable, v2, predicate);
                    copy_or_convert(v1, metable);
                },[&](QJSValue& metable) {
                    p_merge_(metable, v2, predicate);
                });

                if (forInAbleByCastingV1) return v1;

                auto emptyObject = contruct_default_object(v1);
                p_merge_forIn_(emptyObject, v2, predicate);
                copy_or_convert(v1, emptyObject);
                return v1;
            }

            copy_or_convert(v1, v2);

            return v1;
        }
#endif
        /* PRIVATE_MERGE end */

        template <typename Collection, typename Iteratee>
        using p_map_return_type_t = typename Private::rebind_array_value_type<Collection,typename Private::via_func_info<Iteratee, Collection>::non_void_ret_type>::type;

        template <typename Collection, typename Iteratee>
        using p_enable_if_map_return_type_t = typename std::enable_if<
            is_static_collection<Collection>::value && via_func_info<Iteratee,Collection>::is_invokable && !via_func_info<Iteratee,Collection>::is_void_ret,
            p_map_return_type_t<Collection,Iteratee>
        >::type;

        template <typename Collection, typename Iteratee>
        inline auto p_map_(const Collection& collection, Iteratee iteratee) -> p_enable_if_map_return_type_t<Collection,Iteratee> {
            using func_info = Private::via_func_info<Iteratee, Collection>;

            typename Private::rebind_array_value_type<Collection, typename func_info::non_void_ret_type>::type res;

            res.reserve(static_cast<int>(collection.size()));

            for (unsigned int i = 0 ; i < static_cast<unsigned int>(collection.size()) ; i++) {
                res.push_back(Private::invoke(iteratee, collection[i], i, collection));
            }

            return res;
        }

#ifdef QT_CORE_LIB
        template <typename QtMetable>
        QVariantMap _omit(const QtMetable& object, const QStringList& paths) {
            QVariantMap properties;
            for (auto path: paths) {
               p_set_(properties, path , true);
            }

            class OmitPrdicate {
            public:
                QVariantMap properties;
                bool predicate(const QString& key) const {
                    return !(properties.contains(key) && properties[key].toBool());
                }

                OmitPrdicate next(const QString& key) const {
                    OmitPrdicate n;
                    n.properties = properties[key].toMap();
                    return n;
                }
            };
            OmitPrdicate predicate;
            predicate.properties = properties;
            QVariantMap result;
            p_merge_(result, object, predicate);
            return result;
        }

        template <typename QMetaObject>
        inline QVariantMap p_pick_(const QMetaObject& object, const QStringList& paths) {
            QVariantMap tmp;
            QVariantMap res;
            for (auto path: paths) {
                QVariant v = p_get_(object, path, QVariant());
                if (v.isNull()) {
                    continue;
                }
                p_set_(tmp, path , v);
            }
            MergePredicateObject predicate;
            p_merge_(res, tmp, predicate);
            return res;
        }

        template <typename T>
        inline void p_registerQtType_() {
            Private::QtMetaObject metaObject;
            metaObject.count = [](const QVariant& v) {
                const QList<T>* ptr = static_cast<const QList<T>*>(v.constData());
                return ptr->size();
            };

            metaObject.get = [](const QVariant &v, unsigned int index) {
                const QList<T>* ptr = static_cast<const QList<T>*>(v.constData());
                QVariant tmp = QVariant::fromValue((*ptr)[index]);
                return tmp;
            };

            metaObject.create = []() {
                return QVariant::fromValue<T>(T());
            };

            metaObject.toVariantList = [](const QVariant& v) {
                QVariantList res;
                const QList<T>* ptr = static_cast<const QList<T>*>(v.constData());
                for (int i = 0; i < ptr->size(); i++) {
                    res << QVariant::fromValue<T>((*ptr)[i]);
                }
                return res;
            };

            metaObject.fromVariantList = [](const QVariant &v) {
                MergePredicateObject predicate;
                QVariantList list = v.toList();
                auto res = p_map_(list, [=](const QVariant& elem) {
                    T t;
                    p_merge_(t, elem.toMap(), predicate);
                    return t;
                });
                return QVariant::fromValue(res);
            };

            auto& listTable = Private::QtMetaObjectTable<Private::QtMetaObject::List>::storage();
            listTable[qMetaTypeId<QList<T>>()] = metaObject;
        }
#endif
    } /* PRIVATE_END */

    template <typename Object, typename Functor>
    inline const Object& forIn(const Object& object, Functor iteratee) {
        Private::p_forIn_(object, iteratee);
        return object;
    }

    template <typename Object, typename Functor>
    inline Object& forIn(Object& object, Functor iteratee) {
        static_assert(_::Private::is_key_value_type<Object>::value || _::Private::is_qvariant<Object>::value, "_::forIn: " _underline_invalid_argument_type_it_must_be_a_valid_key_value_type);
        Private::p_forIn_(object, iteratee);
        return object;
    }

    template <typename Array, typename Iteratee>
    inline Array& forEach(Array& collection, Iteratee iteratee) {
        static_assert(Private::via_func_info<Iteratee, Array>::is_invokable, "_::forEach(): " _underline_iteratee_mismatched_error);

        return Private::p_forEach_(collection, iteratee);
    }

    template <typename Array, typename Iteratee>
    inline const Array& forEach(const Array& collection, Iteratee iteratee) {
        static_assert(Private::via_func_info<Iteratee, Array>::is_invokable, "_::forEach(): " _underline_iteratee_mismatched_error);

        return Private::p_forEach_(collection, iteratee);
    }

    template <typename Object, typename Source>
    inline auto assign(Object& object, const Source& source) -> typename Private::pointer_or_reference_t<Object> {

        using OBJECT_TYPE = typename Private::key_value_info<Object>;
        using SOURCE_TYPE = typename Private::key_value_info<Source>;

        _underline_static_assert_is_object_source_key_matched("_::assign: ", typename OBJECT_TYPE::key_type, typename SOURCE_TYPE::key_type);

        _underline_static_assert_is_object_source_value_matched("_::assign: ", typename OBJECT_TYPE::value_type, typename SOURCE_TYPE::value_type);

        _underline_static_assert_is_key_value_type("_::assign: ", Object);

        _underline_static_assert_is_key_value_type("_::assign: ", Source);

        static_assert( !(Private::is_qjsvalue<Object>::value && !Private::is_qjsvalue<Source>::value),
                      "_::assign(QJSValue): " _underline_qjsvalue_set_error);

        using Key = typename Private::key_value_info<Source>::key_type;
        using Value = typename Private::key_value_info<Source>::value_type;

        if (Private::is_null_ptr(object)) {
            return object;
        }

        forIn(source, [&](const Value& value, const Key& key) {
            Private::write(object, key, value);
        });

        return object;
    }

    template <typename Dest, typename Source, typename... Args>
    inline auto assign(Dest& dest, const Source& source, Args... sources) -> typename std::enable_if< (sizeof...(Args) > 0), Dest&>::type {
        assign(dest, source);
        assign(dest, sources...);
        return dest;
    }

#ifdef QT_CORE_LIB
    template <typename Object, typename Source>
    inline auto merge(Object& object, const Source& source) -> typename std::conditional<std::is_pointer<Object>::value, Object, Object&>::type {

        using OBJECT_TYPE = typename Private::key_value_info<Object>;
        using SOURCE_TYPE = typename Private::key_value_info<Source>;

        static_assert( !(Private::is_qjsvalue<Object>::value && !Private::is_qjsvalue<Source>::value),
                      "_::merge(QJSValue, source): " _underline_qjsvalue_set_error);

        _underline_static_assert_is_object_source_key_matched("_::merge: ", typename OBJECT_TYPE::key_type, typename SOURCE_TYPE::key_type);

        _underline_static_assert_is_object_source_value_matched("_::merge: ", typename OBJECT_TYPE::value_type, typename SOURCE_TYPE::value_type);

        _underline_static_assert_is_static_qt_metable("_::merge: ", Object);

        _underline_static_assert_is_key_value_type("_::merge: ", Source);

        Private::MergePredicateObject predicate;
        Private::p_merge_(object, source, predicate);

        return object;
    }

    template <typename QtMetable>
    inline QVariant get(const QtMetable &object, const QString &path, const QVariant& defaultValue = QVariant())
    {
        _underline_static_assert_is_object_a_qt_metable("_::get: ", QtMetable);
        return Private::p_get_(object, path, defaultValue);
    }

    template <typename QtMetable, typename QtAny>
    inline Private::pointer_or_reference_t<QtMetable> set(QtMetable &object, const QString &path, const QtAny& value)
    {
        static_assert( !(Private::is_qjsvalue<QtMetable>::value && !Private::is_qjsvalue<QtAny>::value),
                      "_::set(QJSValue): " _underline_qjsvalue_set_error);

        Private::p_set_(object, path, value);

        return object;
    }

    template <typename QMetaObject>
    inline QVariantMap pick(QMetaObject& object, const QStringList& paths) {
        return Private::p_pick_(object, paths);
    }

    template <typename QMetaObject>
    inline QVariantMap pick(QMetaObject& object, const QString& path) {
        return pick(object , QStringList{path});
    }

    template <typename QMetable>
    inline QVariantMap omit(const QMetable& object, const QStringList& paths) {
        return Private::_omit(object, paths);
    }

    template <typename QMetable>
    inline QVariantMap omit(const QMetable& object, const QString& path) {
        return Private::_omit(object, QStringList{path});
    }
#endif

    template <typename Collection, typename Predicate>
    inline bool some(const Collection& collection, Predicate predicate) {
        bool res = false;

        static_assert(Private::via_func_info<Predicate, Collection>::is_invokable, "_::some(): " _underline_predicate_mismatched_error);
        static_assert(std::is_same<typename Private::ret_invoke<Predicate, typename Private::collection_value_type<Collection>::type,int, Collection>::type,bool>::value,
                      "_::some(): " _underline_predicate_return_type_mismatch_error);

        for (unsigned int i = 0 ; i < static_cast<unsigned int>(collection.size()) ; i++) {
            if (Private::invoke(predicate, collection[i], i, collection)) {
                res = true;
                break;
            }
        }
        return res;
    }

    template <typename Collection, typename Predicate>
    inline auto filter(const Collection& collection, Predicate predicate) -> Collection {

        Collection res;

        static_assert(Private::via_func_info<Predicate, Collection>::is_invokable, "_::filter(): " _underline_predicate_mismatched_error);

        static_assert(std::is_same<typename Private::ret_invoke<Predicate, typename Private::collection_value_type<Collection>::type,int, Collection>::type,bool>::value,
                      "_::filter(): " _underline_predicate_return_type_mismatch_error);

        for (unsigned int i = 0 ; i < static_cast<unsigned int>(collection.size()) ; i++) {
            auto v = collection[i];
            if (Private::invoke(predicate, v, i, collection)) {
                res.push_back(v);
            }
        }

        return res;
    }

    template <typename Collection, typename Iteratee>
    inline auto map(const Collection& collection, Iteratee iteratee) -> typename Private::rebind_array_value_type<Collection,
        typename Private::via_func_info<Iteratee, Collection>::non_void_ret_type
    >::type {

        using func_info = Private::via_func_info<Iteratee, Collection>;
        _underline_static_assert_is_collection("_::map(): ", Collection);
        _underline_static_assert_is_iteratee_invokable("_::map(): ", func_info::is_invokable);
        _underline_static_assert_is_iteratee_not_void("_::map() ", !func_info::is_void_ret);
        return Private::p_map_(collection, iteratee);
    }

    template <typename Array,  typename Iteratee>
    inline auto countBy(const Array& collection, Iteratee iteratee) -> typename Private::RebindedMap_keyIterateeRet<Array, Iteratee, int>  {

        _underline_static_assert_is_collection("_::countBy: ", Array);

        _underline_static_assert_is_iteratee_invokable("_::countBy: ", (Private::is_invokable1<Iteratee, _::Private::collection_value_type_t<Array>>::value));

        _underline_static_assert_is_iteratee_not_void("_::countBy: ", (Private::ret_invoke_is_not_void<Iteratee, _::Private::collection_value_type_t<Array>>::value));

        typename Private::RebindedMap_keyIterateeRet<Array, Iteratee, int>  res;

        Private::Value<typename Private::ret_invoke<Iteratee, _::Private::collection_value_type_t<Array>>::type> wrapper;

        for (unsigned int i = 0 ; i < static_cast<unsigned int>(collection.size()) ; i++) {
            wrapper.invoke(iteratee, collection[i]);
            auto key = wrapper.get();
            auto c = res[key] + 1;
            res[key] = c;
        }

        return res;
    }

    template <typename Collection,  typename Iteratee>
    inline auto keyBy(const Collection& collection, Iteratee iteratee) -> typename Private::RebindedMap_keyIterateeRet<Collection, Iteratee, typename Private::collection_value_type<Collection>::type>  {

        _underline_static_assert_is_collection("_::keyBy: ", Collection);

        _underline_static_assert_is_iteratee_invokable("_::keyBy: ", (Private::is_invokable1<Iteratee, _::Private::collection_value_type_t<Collection>>::value));

        _underline_static_assert_is_iteratee_not_void("_::keyBy: ", (Private::ret_invoke_is_not_void<Iteratee, _::Private::collection_value_type_t<Collection>>::value));

        typename Private::RebindedMap_keyIterateeRet<Collection, Iteratee, typename Private::collection_value_type<Collection>::type>  res;

        Private::Value<typename Private::ret_invoke<Iteratee, _::Private::collection_value_type_t<Collection>>::type> wrapper;

        for (unsigned int i = 0 ; i < static_cast<unsigned int>(collection.size()) ; i++) {
            wrapper.invoke(iteratee, collection[i]);
            res[wrapper.get()] = collection[i];
        }

        return res;
    }

    template <typename T>
    T clamp(T number, T lower, T upper) {
        if (number > upper) {
            number = upper;
        }
        if (lower > number) {
            number = lower;
        }
        return number;
    }

    template <typename Collection, typename Iteratee, typename Accumulator>
    inline auto reduce(const Collection& collection, Iteratee iteratee, Accumulator accumulator) -> Accumulator {

        typedef typename Private::ret_invoke<Iteratee,Accumulator, typename Private::collection_value_type<Collection>::type, int, Collection>::type RET;

        static_assert(std::is_same<Accumulator, RET>::value, "_::reduce():  Mismatched accumulator type in reduce() and iteratee().");

        static_assert(Private::is_invokable4<Iteratee,
                                             Accumulator,
                                             typename Private::collection_value_type<Collection>::type,
                                             int, Collection>::value, "_::reduce(): " _underline_iteratee_mismatched_error);

        Accumulator ret = accumulator;
        Private::Value<RET> value;
        for (unsigned int i = 0 ; i < static_cast<unsigned int>(collection.size()) ; i++) {
            value.invoke(iteratee, ret, collection[i], i, collection);
            ret = value.value;
        }
        return ret;
    }

    template <typename Collection, typename V1, typename V2, typename V3>
    Collection range(V1 start, V2 end, V3 step) {
        Collection list;

        int length = std::abs( (end - start) / (step == 0 ? 1 : step) );

        list.reserve(length);

        decltype(start+step) value = start;

        while (length--) {
            list.push_back(value);
            value+=step;
        }
        return list;
    }

    template <typename Collection, typename V1>
    Collection range(V1 end) {
        int step = end > 0 ? 1 : -1;
        return range<Collection>(0, end, step);
    }

    template <typename Collection, typename V1, typename V2>
    Collection range(V1 start, V2 end) {
        int step = start < end ? 1 : -1;
        return range<Collection>(start, end, step);
    }

    template <typename T>
    inline bool isCollection(const T& t) {
        return Private::p_isCollection_(t);
    }

    template <typename T>
    inline bool isMap(const T&) {
        return Private::is_map<T>::value;
    }

    template <typename T>
    inline bool isKeyValueType(const T&) {
        return Private::is_key_value_type<T>::value;
    }

    template <typename T>
    inline bool isQtMetable() {
        return Private::is_static_qt_metable<T>::value;
    }

    template <typename T>
    inline bool isQtMetable(const T&) {
        return Private::is_static_qt_metable<T>::value;
    }

    template <typename ...Args>
    std::vector<int> rangeS(Args ...args) {
        return range<std::vector<int>>(args...);
    }

#ifdef QT_CORE_LIB
    template <typename ...Args>
    QList<int> rangeQ(Args ...args) {
        return range<QList<int>>(args...);
    }

    inline QVariantMap parse(const QString &text)
    {
        QJsonParseError jsonError;
        QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8(),&jsonError);

        if (jsonError.error != QJsonParseError::NoError) {
                qWarning() << "JSON::parse() error: "<< jsonError.errorString();
        }

        return doc.object().toVariantMap();
    }

    template <typename T>
    QString stringify(const T &object, bool indented = false)
    {
        _underline_static_assert_is_object_a_qt_metable("_::stringify: ", T);
        QVariantMap data;
        if (std::is_same<T, QVariantMap>::value) {
            Private::copy_or_convert(data, object);
        } else {
            Private::MergePredicateObject predicate;
            Private::p_merge_(data, object, predicate);
        }

        QJsonObject jsonObject = QJsonObject::fromVariantMap(data);

        QJsonDocument doc;
        doc.setObject(jsonObject);
        QJsonDocument::JsonFormat format = indented ? QJsonDocument::Indented : QJsonDocument::Compact;
        QByteArray bytes = doc.toJson(format);
        return bytes;
    }

    template <typename T>
    inline void registerQtType() {
        _underline_static_assert_is_static_qt_metable("_::registerQtType", T);
        Private::p_registerQtType_<T>();
    };
#endif

    template <typename Object, typename Source, typename Iteratee>
    inline Object assignWith(Object& object, const Source & source, Iteratee iteratee ) {
        using source_info = _::Private::key_value_info<Source>;
        using object_info = _::Private::key_value_info<Object>;

        _underline_static_assert_is_iteratee_invokable("_::assignWith: ", (_::Private::is_invokable5<Iteratee,
                                                        typename object_info::value_type,
                                                        typename source_info::value_type,
                                                        typename source_info::key_type,
                                                        Object, Source>::value));

        static_assert(_::Private::is_key_value_type<Object>::value, "_::assignWith: The first argument must be a valid Map container type where _::isMap() returns true" );

        static_assert(_::Private::is_key_value_type<Source>::value, "_::assignWith: The second argument must be a valid Map container type where _::isMap() returns true" );

        static_assert(std::is_convertible<typename source_info::key_type, typename object_info::key_type>::value,
                      "The key field is not convertible from source to object.");

        forIn(source, [&](const typename source_info::value_type& value,
                         const typename source_info::key_type& key) {
            auto ret = Private::invoke(iteratee,
                            Private::read(object, key),
                            value,
                            key,
                            object,
                            source);
            Private::write(object, key, ret);
        });

        return object;
    }

    template <typename Object>
    inline auto toCollection(const Object& object) -> Private::map_to_collection_rebinder_t<Object, typename Private::key_value_info<Object>::value_type> {
        _underline_static_assert_is_map("_::toCollection: ", Object);

        Private::map_to_collection_rebinder_t<Object, typename Private::key_value_info<Object>::value_type> ret;

        using K = typename Private::key_value_info<Object>::key_type;
        using V = typename Private::key_value_info<Object>::value_type;

        ret.reserve(object.size());
        auto iter = object.begin();
        while (iter != object.end()) {
            ret.push_back(Private::map_iterator_value<K,V>(iter));
            iter++;
        }
        return ret;
    }

    template <typename Collection, typename ValueType>
    inline auto first(const Collection& collection, const ValueType& defaultValue) -> ValueType {
        _underline_static_assert_is_collection("_::first: ", Collection);

        using V = typename Private::collection_info<Collection>::value_type;
        static_assert(std::is_same<V,ValueType>::value, "_::first: " _underline_default_value_type_doesnot_match_with_collection_value_type);

        auto size = Private::collection_info<Collection>::size(collection);
        if (size == 0) {
            return defaultValue;
        } else {
            return Private::collection_info<Collection>::getValue(collection, 0);
        }
    }

    template <typename Collection, typename ValueType>
    inline auto last(const Collection& collection, const ValueType& defaultValue) -> ValueType {
        _underline_static_assert_is_collection("_::first: ", Collection);

        using V = typename Private::collection_info<Collection>::value_type;
        static_assert(std::is_same<V,ValueType>::value, "_::first: " _underline_default_value_type_doesnot_match_with_collection_value_type);

        auto size = Private::collection_info<Collection>::size(collection);
        if (size == 0) {
            return defaultValue;
        } else {
            return Private::collection_info<Collection>::getValue(collection, size - 1);
        }
    }

#ifdef QT_CORE_LIB
    inline auto toCollection(const QVariant &t) -> QVariantList {
        return Private::p_convertToCollection_(t);
    }
#endif

} // End of _ namespace

/* Type Registration */

_underline_register_std_type(std::vector)
_underline_register_std_type(std::list)
_underline_register_std_type(std::map)

_underline_register_rebind_to_map(std::list, std::map)
_underline_register_rebind_to_map(std::vector, std::map)

#ifdef QT_CORE_LIB
_underline_register_qt_container_type(QVector);
_underline_register_qt_container_type(QList);
_underline_register_qt_container_type(QMap);
_underline_register_qt_non_container_type(QStringList);
_underline_register_rebind_to_map(QVector, QMap)
_underline_register_rebind_to_map(QList, QMap)
#endif

/* End of Type Registration */

