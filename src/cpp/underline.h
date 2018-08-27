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
#include <unordered_map>

#ifdef QT_CORE_LIB
#include <QtCore>
#include <QList>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
#include <QObject>
#include <QMetaObject>
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

#define __UNDERLINE_ITERATEE_MISMATCHED_ERROR "Mismatched argument types in the iteratee function. Please validate the number of argument and their type."
#define __UNDERLINE_PREDICATE_MISMATCHED_ERROR "Mismatched argument types in the predicate function. Please validate the number of argument and their type."
#define __UNDERLINE_PREDICATE_RETURN_TYPE_MISMATCH_ERROR "The return type of predicate function must be bool"
#define __UNDERLINE_INPUT_TYPE_IS_NOT_ARRAY "The expected input is an valid array class, where _::isArray() returns true (e.g std::vector , QList , QVector) "
#define __UNDERLINE_ITERATEE_VOID_RET_ERROR "The return type of iteratee function cannot be void"

#define __UNDERLINE_STATIC_ASSERT_IS_ARRAY(prefix, type) \
    static_assert(_::Private::is_array<type>::value, prefix __UNDERLINE_INPUT_TYPE_IS_NOT_ARRAY)

#define __UNDERLINE_STATIC_ASSERT_IS_MAP(prefix, type) \
    static_assert(_::Private::is_map<type>::value, prefix "The expected input is an valid Map container class, where _::isMap() returns true.")

#define __UNDERLINE_STATIC_ASSERT_IS_KEY_VALUE_TYPE(prefix, type) \
    static_assert(_::Private::is_key_value_type<type>::value, prefix "Invalid argument type. It should be a Map container class, or a meta object type likes QObject*, gadget*, QJSValue where _::isKeyValueType() returns true.")

#define __UNDERLINE_STATIC_ASSERT_IS_ITERATEE_INVOKABLE(prefix, value) \
    static_assert(value, prefix __UNDERLINE_ITERATEE_MISMATCHED_ERROR)

#define __UNDERLINE_STATIC_ASSERT_IS_ITERATEE_NOT_VOID(prefix, value) \
    static_assert(value, prefix __UNDERLINE_ITERATEE_VOID_RET_ERROR)

#define __UNDERLINE_STATIC_ASSERT_IS_OBJECT_SOURCE_KEY_MATCHED(prefix, object, source) \
    static_assert(Private::is_custom_convertible<source,object>::value, prefix "The key type of 'source' argument cannot convert to the key type of 'object' argument.")

#define __UNDERLINE_STATIC_ASSERT_IS_OBJECT_SOURCE_VALUE_MATCHED(prefix, object, source) \
    static_assert(Private::is_custom_convertible<source,object>::value, prefix "The value type of 'source' argument cannot convert to the value type of 'object' argument.")

#define UNDERLINE_PRIVATE_NS_BEGIN \
    namespace _ {\
        namespace Private {

#define UNDERLINE_PRIVATE_NS_END \
        } \
    }

#define _DECLARE_UNDERLINE_HAS(name, expr1, expr2) \
        template <typename T> \
        struct has_##name { \
            template <typename Type> \
            static inline auto test(int) -> typename  std::enable_if<std::is_convertible<expr1, expr2>::value, bool>::type; \
            template <typename> \
            static inline auto test(...) -> Undefined; \
            enum { \
                value = !std::is_same<decltype(test<T>(0)), Undefined>::value \
            }; \
        }; \

/// Register rebind_to_map and test_is_array
#define __UNDERLINE_REGISTER_REBIND_TO_MAP(CollectionType, MapType) \
    namespace _ { \
        namespace Private { \
            template <class ValueType, class NewKeyType, class NewValueType> \
            struct array_to_map_rebinder<CollectionType<ValueType>, NewKeyType, NewValueType> { \
                typedef MapType<avoid_void_t<NewKeyType>, avoid_void_t<NewValueType>> type; \
            }; \
        } \
    }

namespace _ {

    namespace Private {

        /// An Undefined class as a default return type of invalid function to keep compiler happy
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
        class QVariantMap: public QMap<QString,QVariant>{};
#endif
#ifndef QT_QUICK_LIB
        class QJSValue{};
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

        _DECLARE_UNDERLINE_HAS(reserve, decltype(std::declval<Type>().reserve(0)), void)

        _DECLARE_UNDERLINE_HAS(push_back, decltype(std::declval<Type>().push_back(std::declval<typename std::remove_reference<typename std::remove_cv<Type>::type::value_type>::type>())), void)

        _DECLARE_UNDERLINE_HAS(static_meta_object, typename std::remove_cv<decltype(std::remove_pointer<Type>::type::staticMetaObject)>::type, QMetaObject)

        _DECLARE_UNDERLINE_HAS(operator_round_backets_int, decltype(std::declval<Type>()[0]), typename std::remove_cv<Type>::type::value_type)

        _DECLARE_UNDERLINE_HAS(operator_round_backets_key, decltype(std::declval<Type>()[std::declval<typename std::remove_cv<Type>::type::key_type>()]), typename std::remove_cv<Type>::type::mapped_type)

        _DECLARE_UNDERLINE_HAS(mapped_type,typename std::remove_cv<Type>::type::mapped_type,typename std::remove_cv<Type>::type::mapped_type)

        _DECLARE_UNDERLINE_HAS(key_type,typename std::remove_cv<Type>::type::key_type,typename std::remove_cv<Type>::type::key_type)

        template <typename T>
        using pointer_or_reference_t = typename std::conditional<std::is_pointer<T>::value, T, T&>::type;

        template <typename From, typename To>
        inline auto convertTo(const From&,  To&) -> typename std::enable_if<!std::is_convertible<From,To>::value, Undefined>::type { return Undefined(); }

        template <typename From, typename To>
        inline auto convertTo(const From& from, To& to) -> typename std::enable_if<std::is_convertible<From,To>::value, void>::type {
            to = from;
        }

#ifdef QT_QUICK_LIB
        inline void convertTo(const QJSValue& from, QVariant& to) {
            to = from.toVariant();
        }
#endif
        template <typename From, typename To>
        struct is_custom_convertible {
            enum {
                value = std::is_convertible<From, To>::value || !std::is_same<Undefined, decltype(convertTo(std::declval<From>(), std::declval<To&>()))>::value
            };
        };

        template <typename T> // Avoid returning a void, change it to Undefined
        using avoid_void_t = typename std::conditional<std::is_same<T,void>::value, Undefined, T>::type;

        template <typename T>
        using remove_cvref_t = typename std::remove_reference<typename std::remove_cv<T>::type>::type;

        template <typename Object>
        struct is_qobject {
            enum { value = std::is_convertible<typename std::add_pointer<typename std::remove_pointer<typename std::remove_cv<Object>::type>::type>::type, QObject*>::value };
        };

        template <typename T> struct is_gadget {
            enum { value = has_static_meta_object<T>::value && ! is_qobject<T>::value};
        };

        template <typename T> struct is_qjsvalue {
            enum { value = std::is_same<T, QJSValue>::value};
        };

        template <typename T> struct is_qany_type {
            enum {
                value = std::is_same<T,QJSValue>::value || std::is_same<T,QVariant>::value
            };
        };

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
        inline auto cast_to_qobject(const T& t) -> typename std::enable_if<std::is_pointer<T >::value && is_qobject<T>::value, QObject*>::type {
            return t;
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
        inline auto can_cast_to_qvariantmap(const T&) -> typename std::enable_if<!is_qany_type<T>::value, bool>::type {
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
            return t.canConvert<QVariantMap>();
        }


#endif
#ifdef QT_QUICK_LIB
        inline auto cast_to_qvariantmap(const QJSValue& t) -> QVariantMap {
            return t.toVariant().toMap();
        }
        inline auto can_cast_to_qvariantmap(const QJSValue& t) -> bool {
            return t.isObject();
        }
#endif

        template <typename T>
        inline auto cast_to_gadget_container(const T&) -> typename std::enable_if<!std::is_same<T, QVariant>::value, GadgetContainer>::type {
            return GadgetContainer();
        }

#ifdef QT_CORE_LIB
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

        inline const char * cast_to_const_char(const char * value) {
            return value;
        }

        template <typename T>
        inline auto cast_to_const_char(const T& string) -> typename std::enable_if<std::is_same<T, QString>::value, const char*>::type {
            return string.toUtf8().constData();
        }

        template <typename T>
        struct is_jsvalue {
            enum {
                value = std::is_same<T, QJSValue>::value
            };
        };

        template <typename ...Args>
        struct _meta_object_info {
            enum {
                is_meta_object = false
            };
            using key_type = Undefined;
            using value_type = Undefined;
        };

        template <typename T>
        struct _meta_object_info<T,
                typename std::enable_if<has_static_meta_object<T>::value || std::is_same<T, GadgetContainer>::value
                , std::true_type>::type> {
            enum { is_meta_object = true};
            using key_type = QString;
            using value_type = QVariant;
        };

        template <typename T>
        struct _meta_object_info<T, typename std::enable_if<std::is_same<T, QJSValue>::value, std::true_type>::type> {
            enum { is_meta_object = true};
            using key_type = QString;
            using value_type = QJSValue;
        };

        template <typename T>
        struct meta_object_info : _meta_object_info<T, std::true_type>{};

        template <typename Meta, typename Key>
        inline auto meta_object_value(const Meta& meta, const Key& key) -> typename std::enable_if<is_gadget<Meta>::value, QVariant>::type {
            auto ptr = cast_to_pointer<Meta>(meta);
            auto k = cast_to_const_char(key);
            auto metaObject = ptr->staticMetaObject;
            int index = metaObject.indexOfProperty(k);
            if (index < 0 ) {
                return QVariant();
            }
            auto property = metaObject.property(index);
            return property.readOnGadget(ptr);
        }

        template <typename Meta, typename Key>
        inline auto meta_object_value(const Meta& meta, const Key& key) -> typename std::enable_if<is_qobject<Meta>::value, QVariant>::type {
            return meta->property(cast_to_const_char(key));
        }

        template <typename Meta, typename Key>
        inline auto meta_object_value(const Meta& meta, const Key& key) -> typename std::enable_if<is_qjsvalue<Meta>::value, QJSValue>::type {
            return meta.property(cast_to_const_char(key));
        }

#ifdef QT_CORE_LIB
        template <typename Meta, typename Key>
        inline auto meta_object_value(const Meta& gadget, const Key& key) -> typename std::enable_if<std::is_same<GadgetContainer, Meta>::value, QVariant>::type {
            if (gadget.constData == nullptr) {
                return QVariant();
            }
            auto k = cast_to_const_char(key);
            int index = gadget.metaObject->indexOfProperty(k);
            if (index < 0) return QVariant();

            QMetaProperty prop = gadget.metaObject->property(index);

            return prop.readOnGadget(gadget.constData);
        }
#endif

        template <typename Meta, typename Key, typename Value>
        inline auto meta_object_set_value(Meta& meta, const Key& key, const Value& value) -> typename std::enable_if<is_gadget<Meta>::value, bool>::type {
            auto ptr = cast_to_pointer<Meta>(meta);
            auto metaObject = ptr->staticMetaObject;
            auto k = cast_to_const_char(key);
            int index = metaObject.indexOfProperty(k);
            if (index < 0 ) {
                return false;
            }
            auto property = metaObject.property(index);
            property.writeOnGadget(ptr, value);
            return true;
        }


        template <typename Meta, typename Key, typename Value>
        inline auto meta_object_set_value(Meta& meta, const Key& key, const Value& value) -> typename std::enable_if<is_qobject<Meta>::value, bool>::type {
            return meta->setProperty(cast_to_const_char(key), value);
        }

        template <typename Meta, typename Key, typename Value>
        inline auto meta_object_set_value(Meta& meta, const Key& key, const Value& value) -> typename std::enable_if<is_qjsvalue<Meta>::value, bool>::type {
            meta.setProperty(cast_to_const_char(key), value);
            return true;
        }


#ifdef QT_CORE_LIB
        template <typename Meta, typename Key, typename Value>
        inline auto meta_object_set_value(Meta& gadget, const Key& key, const Value& value) -> typename std::enable_if<std::is_same<GadgetContainer, Meta>::value, bool>::type {
            if (gadget.data == nullptr) {
                return false;
            }
            auto k = cast_to_const_char(key);
            int index = gadget.metaObject->indexOfProperty(k);
            if (index < 0) return false;

            QMetaProperty prop = gadget.metaObject->property(index);
            prop.writeOnGadget(gadget.data, value);
            return true;
        }

#endif

        template <typename T>
        struct is_array {
            enum {
                value = std::is_class<T>::value &&
                        has_push_back<remove_cvref_t<T>>::value &&
                        has_operator_round_backets_int<remove_cvref_t<T>>::value &&
                        has_reserve<remove_cvref_t<T>>::value
            };
        };

        template <typename T>
        struct is_map {
            enum {
                value = has_key_type<T>::value &&
                        has_mapped_type<T>::value &&
                        has_operator_round_backets_key<T>::value
            };
        };

        template <typename ...Args>
        struct _map_info {
            typedef Undefined key_type;
            typedef Undefined mapped_type;
        };

        template <typename T>
        struct _map_info<T, typename std::enable_if<is_map<T>::value,  std::true_type>::type> {
            typedef typename T::key_type key_type;
            typedef typename T::mapped_type mapped_type;
        };

        template <typename T>
        struct map_info: public _map_info<T, std::true_type> {
        };

        template <typename T>
        using map_key_type_t = typename map_info<T>::key_type;

        template <typename T>
        using map_mapped_type_t = typename map_info<T>::mapped_type;

        template <typename T, typename Key>
        struct is_map_key_matched {
            enum {
                value = is_map<T>::value && std::is_convertible<Key, map_key_type_t<T>>::value
            };
        };

        template <typename T, typename Key, typename Value>
        struct is_map_key_value_matched {
            enum {
                value = is_map<T>::value &&
                std::is_convertible<Key, map_key_type_t<T>>::value &&
                std::is_convertible<Value, map_mapped_type_t<T>>::value
            };
        };

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
        inline auto map_value(const Map& map, const map_key_type_t<Map> &key) -> map_mapped_type_t<Map> {
            map_mapped_type_t<Map> ret = map_mapped_type_t<Map>();
            auto iter = map.find(key);
            if (iter != map.end()) {
                ret = map_iterator_value<map_key_type_t<Map>, map_mapped_type_t<Map> >(iter);
            }
            return ret;
        }

        template <typename T, typename Ret>
        using enable_if_is_map = typename std::enable_if<is_map<T>::value, Ret>::type;

        template <typename T>
        using enable_if_is_map_ret_mapped_type = typename std::enable_if<is_map<T>::value, map_mapped_type_t<T>>;

        template <typename T, typename Key>
        using enable_if_is_map_key_matched = typename std::enable_if<is_map_key_matched<T,Key>::value, map_mapped_type_t<T>>;

        template <typename T, typename Key, typename Value, typename Ret>
        using enable_if_is_map_key_value_matched_ret = typename std::enable_if<is_map_key_value_matched<T, Key, Value>::value, Ret>;

        template <typename T>
        using enable_if_is_array_ret_value_type = typename std::enable_if< is_array<typename std::remove_reference<T>::type>::value, typename std::remove_reference<T>::type::value_type>;

        template <typename Meta>
        struct is_meta_object {
            enum {
                value = meta_object_info<Meta>::is_meta_object
            };
        };

        template <typename Meta,typename Key>
        struct is_meta_object_key_matched {
            enum {
                value = is_meta_object<Meta>::value &&
                std::is_convertible<Key, typename meta_object_info<Meta>::key_type>::value
            };
        };

        template <typename Meta,typename Key, typename Value>
        struct is_meta_object_key_value_matched {
            enum {
                value = is_meta_object<Meta>::value &&
                std::is_convertible<Key, typename meta_object_info<Meta>::key_type>::value &&
                std::is_convertible<Value, typename meta_object_info<Meta>::value_type>::value
            };
        };

        template <typename Meta, typename Key>
        using enable_if_is_meta_object_key_matched = typename std::enable_if<is_meta_object_key_matched<Meta, Key>::value, typename meta_object_info<Meta>::value_type>;

        template <typename Meta, typename Key, typename Value, typename Ret>
        using enable_if_is_meta_object_key_value_matched_ret = typename std::enable_if<is_meta_object_key_value_matched<Meta, Key, Value>::value, Ret>;

        template <typename ...Args>
        struct _key_value_type_info {
            enum {
                is_key_value_type = false
            };
            using key_type = Undefined;
            using value_type = Undefined;
        };

        template <typename T>
        struct _key_value_type_info<T, typename std::enable_if<is_meta_object<T>::value, std::true_type>::type> {
            enum { is_key_value_type = true};
            using key_type = typename meta_object_info<T>::key_type;
            using value_type = typename meta_object_info<T>::value_type;
        };

        template <typename T>
        struct _key_value_type_info<T, typename std::enable_if<is_map<T>::value, std::true_type>::type> {
            enum { is_key_value_type = true};
            using key_type = typename map_info<T>::key_type;
            using value_type = typename map_info<T>::mapped_type;
        };

        template <typename T>
        struct key_value_type: public _key_value_type_info<T, std::true_type> {};

        template <typename T>
        struct is_key_value_type {
            enum { value = key_value_type<T>::is_key_value_type};
        };

        template <typename T>
        struct is_real_key_value_type {
            enum { value = key_value_type<T>::is_key_value_type && !std::is_same<T,QJSValue>::value};
        };

        /// Source: https://stackoverflow.com/questions/5052211/changing-value-type-of-a-given-stl-container
        template <class Container, class NewType>
        struct array_rebinder {
            using type = NullArray<Undefined>;
        };

        template <class ValueType, class... Args, template <class...> class Container, class NewType>
        struct array_rebinder<Container<ValueType, Args...>, NewType>
        {
            using type = Container<NewType, typename array_rebinder<Args, NewType>::type...>;
        };

        template <class Collection, class NewKeyType, class NewValueType>
        struct array_to_map_rebinder {
            using type = NullMap<NewKeyType, NewValueType>;
        };

        /// Check is the Functor be able to take Args as input. It works with generic lambda.
        template <typename Functor,typename ...Args>
        struct is_args_compatible {
            enum {
                value = std::is_convertible<Functor, std::function<void(Args...)> >::value
            };
        };

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

        template <typename ...Args>
        struct array_info {
            typedef Undefined size_type;
            typedef Undefined value_type;
        };

        template <typename T>
        struct array_info<T, typename std::enable_if<is_array<remove_cvref_t<T>>::value, bool>::type> {
            using size_type = typename remove_cvref_t<T>::size_type;
            using value_type = typename remove_cvref_t<T>::value_type;
        };

        template <typename Array>
        struct array_value_type {
            using type = typename array_info<Array, bool>::value_type;
        };

        template <typename Array>
        using array_value_type_t = typename array_info<Array, bool>::value_type;

        template <typename Array>
        struct array_size_type {
            using type = typename array_info<Array, bool>::size_type;
        };

        template <typename Collection, typename Index>
        struct is_array_index_matched {
            enum {
                value = is_array<Collection>::value && std::is_convertible<Index, int>::value
            };
        };

        template <typename Collection, typename Index>
        using enable_if_collection_index_matched = std::enable_if<is_array_index_matched<Collection, Index>::value, typename array_value_type<Collection>::type>;

        /// Read a property from the target container object
        template <typename Map, typename Key>
        inline auto read(const Map &map, Key key) ->
            typename enable_if_is_map_key_matched<Map, Key>::type {
            return map_value<Map>(map, key);
        }

        template <typename Collection, typename Index>
        inline auto read(const Collection& collection, Index index) -> typename enable_if_collection_index_matched<Collection, Index>::type {
            return collection[index];
        }

        template <typename Meta, typename Key>
        inline auto read(const Meta& meta, const Key &key) -> typename enable_if_is_meta_object_key_matched<Meta, Key>::type {
            return meta_object_value(meta, key);
        }

        template <typename Any, typename Key>
        struct is_readable {
            enum {
                value = is_meta_object_key_matched<Any, Key>::value ||
                is_map_key_matched<Any, Key>::value ||
                is_array_index_matched<Any, Key>::value
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
            typename enable_if_is_map_key_value_matched_ret
<Map, Key, Value, bool>::type {
            map[key] = value;
            return true;
        }

        template <typename Map, typename Key, typename Value>
        inline auto write(Map &map, const Key& key, const Value& value) ->
            typename enable_if_is_meta_object_key_value_matched_ret
<Map, Key, Value, bool>::type {
            meta_object_set_value(map, key, value);
            return true;
        }

        template <typename Map, typename Key, typename Value>
        inline auto write(Map &&map, const Key& key, const Value& value) ->
            typename enable_if_is_meta_object_key_value_matched_ret
<Map, Key, Value, bool>::type {
            meta_object_set_value(map, key, value);
            return true;
        }

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
        using ret_invoke_collection_value_type_t = typename ret_invoke<Iteratee, typename array_value_type<Collection>::type>::type;

        template <class Collection, typename Iteratee, typename ValueType>
        using rebind_to_map_collection_iteratee_t = typename array_to_map_rebinder<remove_cvref_t<Collection>,   _::Private::ret_invoke_collection_value_type_t<Iteratee,remove_cvref_t<Collection>>, ValueType>::type;

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
                return (T) other == value;
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

            using value_type = typename array_value_type<Array>::type;
            using size_type = typename array_size_type<Array>::type;

            using ret_type = typename ret_invoke<Functor, value_type, size_type, Array>::type;

            using non_void_ret_type = typename std::conditional<std::is_same<void, ret_type>::value, Undefined, ret_type>::type;

            enum {
                is_invokable = is_invokable3<Functor, value_type, size_type, Array>::value,
                is_void_ret = std::is_same<typename ret_invoke<Functor,value_type,size_type,Array>::type, void>::value
            };

        };

#ifdef QT_CORE_LIB

        template <class NewType>
        struct array_rebinder<QStringList, NewType> {
            typedef QList<NewType> type;
        };

        template <class NewType>
        struct array_rebinder<QVariantList, NewType> {
            typedef QList<NewType> type;
        };

        inline QVariant _get(const QVariantMap& object, const QStringList &path, const QVariant& defaultValue) ;

        inline QVariant _get(const QObject* object, const QStringList &path, const QVariant& defaultValue) {

            QString key = path[0];

            const QMetaObject* meta = object->metaObject();

            if (meta->indexOfProperty(key.toUtf8().constData()) < 0) {
                return defaultValue;
            }

            QVariant value = object->property(key.toUtf8().constData());

            if (path.size() == 1) {
                return value;
            }

            QStringList nextPath = path;
            nextPath.removeFirst();

            if (value.canConvert<QObject*>()) {
                return _get(qvariant_cast<QObject*>(value), nextPath, defaultValue);
            } else if (value.type() == QVariant::Map) {
                return _get(value.toMap(), nextPath, defaultValue);
            } else {
                return defaultValue;
            }
        }

        inline QVariant _get(const QVariantMap& object, const QStringList &path, const QVariant& defaultValue) {

            QString key = path[0];

            if (!object.contains(key)) {
                return defaultValue;
            }

            QVariant value = object[key];

            if (path.size() == 1) {
                return value;
            }

            QStringList nextPath = path;
            nextPath.removeFirst();

            if (value.canConvert<QObject*>()) {
                return _get(qvariant_cast<QObject*>(value), nextPath, defaultValue);
            } else if (value.type() == QVariant::Map) {
                return _get(value.toMap(), nextPath, defaultValue);
            } else {
                return defaultValue;
            }
        }
#endif

        template <typename Map, typename Functor>
        inline auto forIn(Map& object, Functor iteratee) -> typename std::enable_if<Private::is_map<Map>::value, Map&>::type {

            using K = typename Private::map_info<Map>::key_type;
            using V = typename Private::map_info<Map>::mapped_type;

            static_assert(Private::is_invokable3<Functor, V, K, Map>::value, "_::forIn: " __UNDERLINE_ITERATEE_MISMATCHED_ERROR);

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

        template <typename Map, typename Functor>
        inline auto forIn(const Map& object, Functor iteratee) -> typename std::enable_if<Private::is_map<Map>::value, const Map&>::type {
            using K = typename Private::map_info<Map>::key_type;
            using V = typename Private::map_info<Map>::mapped_type;

            static_assert(Private::is_invokable3<Functor, V, K, Map>::value, "_::forIn: " __UNDERLINE_ITERATEE_MISMATCHED_ERROR);

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

        template <typename Value, typename Iteratee>
        inline auto forIn(const Value& object, Iteratee iteratee)
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

        template <typename Object, typename Functor>
        inline auto forIn(const Object* object, Functor iteratee) -> typename std::enable_if<Private::is_qobject<Object>::value, const Object*>::type {
            const QMetaObject* meta = object->metaObject();
            static_assert(Private::is_invokable3<Functor, QVariant, QString, QObject*>::value, "_::forIn: " __UNDERLINE_ITERATEE_MISMATCHED_ERROR);
            Private::Value<typename Private::ret_invoke<Functor, QVariant, QString, QObject*>::type> invokeHelper;

            for (int i = 0 ; i < meta->propertyCount(); i++) {
                const QMetaProperty property = meta->property(i);
                QString key = property.name();
                QVariant value = property.read(object);

                invokeHelper.invoke(iteratee, value, key, object);

                if (invokeHelper.template canConvert<bool>() && invokeHelper.equals(false)) {
                    break;
                }
            }
            return object;
        }

        template <typename Object, typename Functor>
        inline auto forIn(const Object& object, Functor iteratee) -> typename std::enable_if<Private::is_gadget<Object>::value, const Object&>::type {
            auto ptr = cast_to_pointer(object);

            const QMetaObject meta = ptr->staticMetaObject;

            static_assert(Private::is_invokable3<Functor, QVariant, QString, const Object&>::value, "_::forIn: " __UNDERLINE_ITERATEE_MISMATCHED_ERROR);
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
        inline auto forIn(const Object& object, Functor iteratee) -> typename std::enable_if<std::is_same<Object, GadgetContainer>::value, const Object&>::type {
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

        /* PRIVATE_MERGE begin */

        template <typename V1, typename V2>
        inline void forIn_merge(V1 &v1 , const V2& v2);

        template <typename V1, typename V2>
        inline auto merge(const V1&, const V2& v2) ->
        typename std::enable_if<
            !(is_key_value_type<V1>::value && is_key_value_type<V2>::value)  &&
            !(std::is_same<V1,QVariant>::value) &&
            !(std::is_same<V1,QJSValue>::value) &&
            !(std::is_same<V2,QJSValue>::value)
            ,V2>::type { // The default merge function which is not doing anything
            return v2;
        }

        template <typename V2>
        inline QVariant merge(QVariant &v1, const V2 &v2);

        template <typename V1, typename V2>
        inline auto merge(V1& v1, const V2& v2) -> typename std::enable_if<is_real_key_value_type<V1>::value && is_real_key_value_type<V2>::value, pointer_or_reference_t<V1> >::type { // Generic merge function
            forIn_merge(v1, v2);
            return v1;
        }

        template <typename V1, typename V2>
        inline auto merge(V1& v1, const V2& v2) ->
            typename std::enable_if<is_real_key_value_type<V1>::value && !is_real_key_value_type<V2>::value, V1&>::type {

            auto ptr = cast_to_qobject(v2);

            if (ptr) {
                forIn_merge(v1, ptr);
                return v1;
            }

            GadgetContainer gadget = cast_to_gadget_container(v2);
            if (gadget.metaObject != nullptr) {
                merge(v1, gadget);
                return v1;
            }

            auto map = cast_to_qvariantmap(v2);
            forIn_merge(v1, map);
            return v1;
        }

#ifdef QT_CORE_LIB
        template <typename V2>
        inline QVariant merge(QVariant &v1, const V2 &v2) {
            QObject* qobject = cast_to_qobject(v1);
            if (qobject != nullptr) {
                merge(qobject, v2);
                return v1;
            }

            GadgetContainer gadget = cast_to_gadget_container(v1);
            if (gadget.metaObject != nullptr) {
                merge(gadget, v2);
                return v1;
            }

            if (v1.canConvert<QVariantMap>()){
                auto map = v1.toMap();
                merge(map, v2);
                v1 = map;
                return v1;
            }

            qobject = cast_to_qobject(v2);
            if (qobject != nullptr) {
                auto map = v1.toMap();
                return merge(map, qobject);
            }

            gadget = cast_to_gadget_container(v2);
            if (gadget.metaObject != nullptr) {
                auto map = v1.toMap();
                return merge(map, gadget);
            }

            QVariant value;
            convertTo(v2, value);
            return value;
        }
#endif

#ifdef QT_QUICK_LIB

        template <typename V2>
        inline auto merge(QJSValue& v1, const V2 & v2) -> typename std::enable_if<std::is_same<V2, QJSValue>::value, QJSValue&>::type {
            if (v1.isQObject()) {
                auto ptr = v1.toQObject();
                merge(ptr, v2);
                return v1;
            } else if (v1.isObject()) {
                forIn_merge(v1, v2);
                return v1;
            } else {
                v1 = v2;
                return v1;
            }
        }
#endif

        template <typename V1, typename V2>
        inline void forIn_merge(V1& v1 , const V2& v2) {
            using Key = typename key_value_type<V2>::key_type;
            using Value = typename key_value_type<V2>::value_type;

            forIn(v2, [&](const Value& value, const Key& key) {
                auto srcValue = read(v1,key);

                QObject* v1_qobject_ptr = cast_to_qobject(srcValue);
                if (v1_qobject_ptr != nullptr) {
                    merge(v1_qobject_ptr, value);
                    return;
                }

                GadgetContainer gadget = cast_to_gadget_container(srcValue);
                if (gadget.metaObject != nullptr) {
                    merge(gadget, value);
                    write(v1, key, srcValue);
                    return;
                }

                write(v1, key, merge(srcValue, value));
            });
        }

        /* PRIVATE_MERGE end */

    } /* End of Private Session */

    template <typename Object, typename Functor>
    inline const Object& forIn(const Object& object, Functor iteratee) {
        Private::forIn(object, iteratee);
        return object;
    }

    template <typename Collection, typename Iteratee>
    inline const Collection& forEach(const Collection& collection, Iteratee iteratee) {
        static_assert(Private::via_func_info<Iteratee, Collection>::is_invokable, "_::forEach(): " __UNDERLINE_ITERATEE_MISMATCHED_ERROR);

        Private::Value<typename Private::ret_invoke<Iteratee, typename Private::array_value_type<Collection>::type, int, Collection >::type> value;

        for (unsigned int i = 0 ; i < (unsigned int) collection.size() ; i++) {
            value.invoke(iteratee, collection[i], i, collection);
            if (value.template canConvert<bool>() && value.equals(false)) {
                break;
            }
        }
        return collection;
    }

#ifdef QT_CORE_LIB
    /*
     If a property contains QObject pointer, it will be converted to QVariantMap.

     In case you need to obtain a QObject pointer, please use get().
     */

    /// Assign properties from source object to the destination object.
    inline QVariantMap& assign(QVariantMap &dest, const QObject *source)
    {
        forIn(source, [&](const QVariant& value, const QString& key) {
            QVariant v = value;

            if (value.canConvert<QObject*>()) {
                QVariantMap map;
                assign(map, value.value<QObject*>()); // nested properties are not supported yet
                v = map;
            }

            dest[key] = v;
        });
        return dest;
    }

    inline QObject* assign(QObject *dest, const QVariantMap & source)
    {
        const QMetaObject* meta = dest->metaObject();

        forIn(source, [&](QVariant value, QString key) {

            int index = meta->indexOfProperty(key.toLocal8Bit().constData());
            if (index < 0) {
                qWarning() << QString("_::assign: assigns an non-existed property: %1").arg(key);
                return;
            }

            QVariant orig = dest->property(key.toLocal8Bit().constData());

            if (orig.canConvert<QObject*>()) {
                if (value.type() != QVariant::Map) {
                    qWarning() << QString("assign:expect a QVariantMap property but it is not: %1");
                } else {
                    assign(orig.value<QObject*>(), value.toMap());
                }

            } else if (orig != value) {
                dest->setProperty(key.toLocal8Bit().constData(), value);
            }
        });

        return dest;
    }

    inline QVariantMap& assign(QVariantMap& dest, const QVariantMap& source) {
        forIn(source, [&](const QVariant& value , const QString& key) {
            dest[key] = value;
        });
        return dest;
    }

    inline QObject* assign(QObject* dest, const QObject* source) {
        forIn(source, [&](const QVariant& value , const QString& key) {
            Private::write(dest, key, value);
        });
        return dest;
    }
#endif

#ifdef QT_QUICK_LIB
    inline QObject * assign(QObject *dest, const QJSValue &source)
    {
        if (dest == 0) {
            return dest;
        }

        const QMetaObject* meta = dest->metaObject();
        QJSValueIterator iter(source);

        while (iter.hasNext()) {
            iter.next();
            QByteArray key = iter.name().toLocal8Bit();
            int index = meta->indexOfProperty(key.constData());
            if (index < 0) {
                qWarning() << QString("QSyncable::assign:assign a non-existed property: %1").arg(iter.name());
                continue;
            }

            QVariant orig = dest->property(key.constData());

            if (orig.canConvert<QObject*>()) {
                if (!iter.value().isObject()) {
                    qWarning() << QString("QSyncable::assign:expect a object property but it is not: %1");
                } else {
                    assign(orig.value<QObject*>(), iter.value());
                }
                continue;
            }

            QVariant value = iter.value().toVariant();
            if (orig != value) {
                dest->setProperty(key.constData(), value);
            }
        }
        return dest;
    }
#endif

    template <typename Dest, typename Source, typename... Args>
    inline auto assign(Dest& dest, const Source& source, Args... sources) -> typename std::enable_if< (sizeof...(Args) > 0), Dest&>::type {
        assign(dest, source);
        assign(dest, sources...);
        return dest;
    }

    /* End of assign() */

#ifdef QT_CORE_LIB
    inline QVariant get(const QObject *object, const QStringList &path, const QVariant& defaultValue)
    {
        return Private::_get(object, path, defaultValue);
    }

    inline QVariant get(const QObject *object, const QString &path, const QVariant& defaultValue = QVariant())
    {
        return get(object, path.split("."), defaultValue);
    }

    inline QVariant get(const QVariantMap &source, const QStringList &path, const QVariant &defaultValue = QVariant())
    {
        return Private::_get(source, path, defaultValue);
    }

    inline QVariant get(const QVariantMap &source, const QString &path, const QVariant &defaultValue = QVariant())
    {
        return get(source, path.split("."), defaultValue);
    }

    inline void set(QVariantMap &data, const QStringList &path, const QVariant &value)
    {
        QString key = path[0];

        if (path.size() == 1) {
            data[key] = value;
        } else {
            if (!data.contains(key) || !data[key].canConvert<QVariantMap>()) {
                data[key] = QVariantMap();
            }
            QStringList nextPath = path;
            nextPath.removeFirst();
            QVariantMap map = data[key].toMap();
            set(map, nextPath, value);
            data[key] = map;
        }
    }

    inline void set(QVariantMap &data, const QString &path, const QVariant &value)
    {
        return set(data, path.split("."), value);
    }
    /// Creates an QVariantMap composed of the picked object properties at paths.
    /*
     Example:

         pick(object, QStringList() << "a" << "b.c");

     If a property contains QObject pointer, it will be converted to QVariantMap.

     In case you need to obtain a QObject pointer, please use get().

     */

    inline QVariantMap pick(QObject *object, const QStringList &paths)
    {
        QVariantMap data;
        foreach (QString path, paths) {
            QVariant value = get(object, path);
            if (value.isNull()) {
                continue;
            }

            if (value.canConvert<QObject*>()) {
                QVariantMap map;
                assign(map, value.value<QObject*>());
                value = map;
            }

            set(data, path, value);
        }
        return data;
    }


    inline QVariantMap pick(QVariantMap source, const QStringList &paths)
    {
        QVariantMap data;
        foreach (QString path, paths) {
            QVariant value = get(source, path);
            if (value.isNull()) {
                continue;
            }

            if (value.canConvert<QObject*>()) {
                QVariantMap map;
                assign(map, value.value<QObject*>());
                value = map;
            }

            set(data, path, value);
        }
        return data;
    }

    inline QVariantMap pick(QVariantMap source, const QVariantMap &paths)
    {
        return pick(source, paths.keys());
    }

    /// The opposite of pick(), this method creates an QVariantMap composed of the own properties that are not omitted.

    inline QVariantMap omit(const QVariantMap &source, const QVariantMap &properties)
    {

        QMap<QString,QVariant>::const_iterator iter = source.begin();
        QVariantMap result;

        while (iter != source.end()) {

            if (properties.contains(iter.key())) {
                iter++;
                continue;
            }

            QVariant value = source[iter.key()];

            if (value.canConvert<QObject*>()) {
                QVariantMap map;
                assign(map, value.value<QObject*>());
                value = map;
            }

            result[iter.key()] = value;
            iter++;
        }

        return result;
    }
#endif

    template <typename Collection, typename Predicate>
    inline bool some(const Collection& collection, Predicate predicate) {
        bool res = false;

        static_assert(Private::via_func_info<Predicate, Collection>::is_invokable, "_::some(): " __UNDERLINE_PREDICATE_MISMATCHED_ERROR);
        static_assert(std::is_same<typename Private::ret_invoke<Predicate, typename Private::array_value_type<Collection>::type,int, Collection>::type,bool>::value,
                      "_::some(): " __UNDERLINE_PREDICATE_RETURN_TYPE_MISMATCH_ERROR);

        for (unsigned int i = 0 ; i < (unsigned int) collection.size() ; i++) {
            if (Private::invoke(predicate, collection[i], i, collection)) {
                res = true;
                break;
            }
        }
        return res;
    }

    template <typename Collection, typename Iteratee>
    inline auto map(const Collection& collection, Iteratee iteratee) -> typename Private::array_rebinder<Collection,
        typename Private::via_func_info<Iteratee, Collection>::non_void_ret_type
    >::type {

        using func_info = Private::via_func_info<Iteratee, Collection>;

        __UNDERLINE_STATIC_ASSERT_IS_ARRAY("_::map(): ", Collection);

        __UNDERLINE_STATIC_ASSERT_IS_ITERATEE_INVOKABLE("_::map(): ", func_info::is_invokable);

        __UNDERLINE_STATIC_ASSERT_IS_ITERATEE_NOT_VOID("_::map() ", !func_info::is_void_ret);

        typename Private::array_rebinder<Collection, typename func_info::non_void_ret_type>::type res;

        res.reserve((int) collection.size());

        for (unsigned int i = 0 ; i < (unsigned int) collection.size() ; i++) {
            res.push_back(Private::invoke(iteratee, collection[i], i, collection));
        }

        return res;
    }

    template <typename Collection,  typename Iteratee>
    inline auto countBy(const Collection& collection, Iteratee iteratee) -> typename Private::rebind_to_map_collection_iteratee_t<Collection, Iteratee, int>  {

        __UNDERLINE_STATIC_ASSERT_IS_ARRAY("_::countBy: ", Collection);

        __UNDERLINE_STATIC_ASSERT_IS_ITERATEE_INVOKABLE("_::countBy: ", (Private::is_invokable1<Iteratee, _::Private::array_value_type_t<Collection>>::value));

        __UNDERLINE_STATIC_ASSERT_IS_ITERATEE_NOT_VOID("_::countBy: ", (Private::ret_invoke_is_not_void<Iteratee, _::Private::array_value_type_t<Collection>>::value));

        typename Private::rebind_to_map_collection_iteratee_t<Collection, Iteratee, int>  res;

        Private::Value<typename Private::ret_invoke<Iteratee, _::Private::array_value_type_t<Collection>>::type> wrapper;

        for (unsigned int i = 0 ; i < (unsigned int) collection.size() ; i++) {
            wrapper.invoke(iteratee, collection[i]);
            auto key = wrapper.get();
            auto c = res[key] + 1;
            res[key] = c;
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

        typedef typename Private::ret_invoke<Iteratee,Accumulator, typename Private::array_value_type<Collection>::type, int, Collection>::type RET;

        static_assert(std::is_same<Accumulator, RET>::value, "_::reduce():  Mismatched accumulator type in reduce() and iteratee().");

        static_assert(Private::is_invokable4<Iteratee,
                                             Accumulator,
                                             typename Private::array_value_type<Collection>::type,
                                             int, Collection>::value, "_::reduce(): " __UNDERLINE_ITERATEE_MISMATCHED_ERROR);

        Accumulator ret = accumulator;
        Private::Value<RET> value;
        for (unsigned int i = 0 ; i < (unsigned int) collection.size() ; i++) {
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
    inline bool isArray() {
        return Private::is_array<T>::value;
    }

    template <typename T>
    inline bool isArray(const T&) {
        return Private::is_array<T>::value;
    }

    template <typename T>
    inline bool isMap() {
        return Private::is_map<T>::value;
    }

    template <typename T>
    inline bool isMap(const T&) {
        return Private::is_map<T>::value;
    }

    template <typename T>
    inline bool isKeyValueType() {
        return Private::is_key_value_type<T>::value;
    }

    template <typename T>
    inline bool isKeyValueType(const T&) {
        return Private::is_key_value_type<T>::value;
    }

#ifdef QT_CORE_LIB
    template <typename ...Args>
    QList<int> range_q(Args ...args) {
        return range<QList<int>>(args...);
    }
#endif

    template <typename Object, typename Source, typename Iteratee>
    inline Object assignWith(Object& object, const Source & source, Iteratee iteratee ) {
        using source_info = _::Private::key_value_type<Source>;
        using object_info = _::Private::key_value_type<Object>;

        __UNDERLINE_STATIC_ASSERT_IS_ITERATEE_INVOKABLE("_::assignWith: ", (_::Private::is_invokable5<Iteratee,
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

    template <typename Object, typename Source>
    inline auto merge(Object& object, const Source& source) -> typename std::conditional<std::is_pointer<Object>::value, Object, Object&>::type {

        using OBJECT_TYPE = typename Private::key_value_type<Object>;
        using SOURCE_TYPE = typename Private::key_value_type<Source>;

        __UNDERLINE_STATIC_ASSERT_IS_OBJECT_SOURCE_KEY_MATCHED("_::merge: ", typename OBJECT_TYPE::key_type, typename SOURCE_TYPE::key_type);

        __UNDERLINE_STATIC_ASSERT_IS_OBJECT_SOURCE_VALUE_MATCHED("_::merge: ", typename OBJECT_TYPE::value_type, typename SOURCE_TYPE::value_type);

        __UNDERLINE_STATIC_ASSERT_IS_KEY_VALUE_TYPE("_::merge: ", Object);

        __UNDERLINE_STATIC_ASSERT_IS_KEY_VALUE_TYPE("_::merge: ", Source);

        static_assert( !(Private::is_qjsvalue<Object>::value && !Private::is_qjsvalue<Source>::value),
                      "_::merge:  _merge(QJSValue) could not take source argument another then the type of QJSValue.");

        Private::merge(object, source);

        return object;
    }

} // End of _ namespace

/* Type Registration */

__UNDERLINE_REGISTER_REBIND_TO_MAP(std::list, std::map)
__UNDERLINE_REGISTER_REBIND_TO_MAP(std::vector, std::map)

#ifdef QT_CORE_LIB
__UNDERLINE_REGISTER_REBIND_TO_MAP(QVector, QMap)
__UNDERLINE_REGISTER_REBIND_TO_MAP(QList, QMap)
#endif

/* End of Type Registration */
