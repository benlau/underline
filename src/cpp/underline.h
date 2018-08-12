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

1. Don't use constexpr together with std::enable_if.

c++ - enable_if not working in Visual Studio when using a constexpr function as argument - Stack Overflow
https://stackoverflow.com/questions/46144103/enable-if-not-working-in-visual-studio-when-using-a-constexpr-function-as-argume

 */

#define UNDERLINE_ITERATEE_MISMATCHED_ERROR "Mismatched argument types in the iteratee function. Please validate the number of argument and their type."
#define UNDERLINE_PREDICATE_MISMATCHED_ERROR "Mismatched argument types in the predicate function. Please validate the number of argument and their type."
#define UNDERLINE_PREDICATE_RETURN_TYPE_MISMATCH_ERROR "The return type of predicate function must be bool"
#define UNDERLINE_INPUT_TYPE_IS_NOT_ARRAY "The expected input is an array class, where _::isArray() returns true (e.g std::vector , QList , QVector) "

#define UNDERLINE_STATIC_ASSERT_IS_ARRAY(prefix, type) \
    static_assert(_::Private::is_array<type>::value, prefix UNDERLINE_INPUT_TYPE_IS_NOT_ARRAY);

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
#define UL_REGISTER_REBIND_TO_MAP(CollectionType, MapType) \
    namespace _ { \
        namespace Private { \
            template <class NewType, class ValueType> \
            struct rebind_to_value_map<CollectionType<ValueType>, NewType> { \
                typedef MapType<ValueType, NewType> type; \
            }; \
            template <class ValueType, class NewKeyType, class NewValueType> \
            struct rebind_to_map_key_value<CollectionType<ValueType>, NewKeyType, NewValueType> { \
                typedef MapType<NewKeyType, NewValueType> type; \
            }; \
        } \
    }

namespace _ {

    namespace Private {

        /// An Undefined class as a default return type of invalid function
        class Undefined {
        };

#ifndef QT_CORE_LIB
        class QObject{};
        class QMetaObject{};
        class QVariant{};
        class QString{};
        template <typename Key, typename Value>
        class QMap {
        public:
            class const_iterator{
            };
        };
#endif
        _DECLARE_UNDERLINE_HAS(reserve, decltype(std::declval<Type>().reserve(0)), void)

        _DECLARE_UNDERLINE_HAS(push_back, decltype(std::declval<Type>().push_back(std::declval<typename std::remove_reference<typename std::remove_cv<Type>::type::value_type>::type>())), void)

        _DECLARE_UNDERLINE_HAS(static_meta_object, typename std::remove_cv<decltype(std::remove_pointer<Type>::type::staticMetaObject)>::type, QMetaObject)

        _DECLARE_UNDERLINE_HAS(operator_round_backets_int, decltype(std::declval<Type>()[0]), typename std::remove_cv<Type>::type::value_type)

        _DECLARE_UNDERLINE_HAS(operator_round_backets_key, decltype(std::declval<Type>()[std::declval<typename std::remove_cv<Type>::type::key_type>()]), typename std::remove_cv<Type>::type::mapped_type)

        _DECLARE_UNDERLINE_HAS(mapped_type,typename std::remove_cv<Type>::type::mapped_type,typename std::remove_cv<Type>::type::mapped_type)

        _DECLARE_UNDERLINE_HAS(key_type,typename std::remove_cv<Type>::type::key_type,typename std::remove_cv<Type>::type::key_type)

        template <typename T>
        using remove_cvref_t = typename std::remove_reference<typename std::remove_cv<T>::type>::type;

        template <typename Object>
        struct is_qobject {
            enum { value = std::is_convertible<typename std::add_pointer<typename std::remove_pointer<typename std::remove_cv<Object>::type>::type>::type, QObject*>::value };
        };

        template <typename T> struct is_gadget {
            enum { value = has_static_meta_object<T>::value && ! is_qobject<T>::value};
        };

        template <typename T>
        auto inline cast_to_pointer(const T& value) -> typename std::enable_if<std::is_pointer<T>::value, const T&>::type {
            return value;
        }

        template <typename T>
        auto inline cast_to_pointer(const T& value) -> typename std::enable_if<!std::is_pointer<T>::value, const T*>::type {
            return &value;
        }

        template <typename Meta>
        struct meta_object_info {
            enum {
                is_meta_object = has_static_meta_object<Meta>::value
            };

            typedef typename std::conditional<has_static_meta_object<Meta>::value, QString, Undefined>::type key_type;
            typedef typename std::conditional<has_static_meta_object<Meta>::value, QVariant, Undefined>::type value_type;
        };

        template <typename Meta, typename Key>
        inline auto meta_object_value(const Meta& meta, const Key& key) -> typename std::enable_if<is_gadget<Meta>::value, QVariant>::type {
            auto ptr = cast_to_pointer<Meta>(meta);
            auto metaObject = ptr->staticMetaObject;
            int index = metaObject.indexOfProperty(key);
            if (index < 0 ) {
                return QVariant();
            }
            auto property = metaObject.property(index);
            return property.readOnGadget(ptr);
        }

        template <typename Meta, typename Key>
        inline auto meta_object_value(const Meta& meta, const Key& key) -> typename std::enable_if<is_qobject<Meta>::value, QVariant>::type {
            return meta->property(key);
        }

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
        struct map_info {
            typedef Undefined key_type;
            typedef Undefined mapped_type;
        };

        template <typename T>
        struct map_info<T, typename std::enable_if<is_map<T>::value,  T>::type> {
            typedef typename T::key_type key_type;
            typedef typename T::mapped_type mapped_type;
        };

        template <typename T>
        using map_key_type_t = typename map_info<T,T>::key_type;

        template <typename T>
        using map_mapped_type_t = typename map_info<T,T>::mapped_type;

        template <typename T, typename Key>
        struct is_map_key_matched {
            enum {
                value = is_map<T>::value && std::is_convertible<Key, map_key_type_t<T>>::value
            };
        };

        template <typename Key, typename Value>
        inline Value map_iterator_value(typename std::map<Key,Value>::const_iterator & iter) {
            return iter->second;
        }

        template <typename Key, typename Value>
        inline Value map_iterator_value(typename QMap<Key,Value>::const_iterator & iter) {
            return iter.value();
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
                value = is_meta_object<Meta>::value && std::is_convertible<Key, typename meta_object_info<Meta>::key_type>::value
            };
        };

        template <typename Meta, typename Key>
        using enable_if_is_meta_object_key_matched = typename std::enable_if<is_meta_object_key_matched<Meta, Key>::value, typename meta_object_info<Meta>::value_type>;

        /// Source: https://stackoverflow.com/questions/5052211/changing-value-type-of-a-given-stl-container
        template <class Container, class NewType>
        struct rebind {
            using type = Undefined;
        };

        template <class ValueType, class... Args, template <class...> class Container, class NewType>
        struct rebind<Container<ValueType, Args...>, NewType>
        {
            typedef Container<NewType, typename rebind<Args, NewType>::type...> type;
        };

        template <class Collection, class NewType>
        struct rebind_to_value_map {
            typedef Undefined type;
        };

        template <class Collection, class NewKeyType, class NewValueType>
        struct rebind_to_map_key_value {
            typedef Undefined type;
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

        template <typename Functor, typename ...Args>
        struct ret_func {
            using type = decltype(decl_func0<Functor, Args&&...>());
        };

        template <typename Collection>
        typename std::enable_if<is_array<Collection>::value, typename std::remove_reference<Collection>::type::value_type>::type decl_array_value_type();

        template <typename Collection>
        typename std::enable_if<!is_array<Collection>::value, Undefined>::type decl_array_value_type();

        template <typename Collection>
        struct array_value_type {
            using type = decltype(decl_array_value_type<remove_cvref_t<Collection>>());
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

        /* END_GENERATED_CODE */

        template <typename Functor, typename ...Args>
        struct ret_invoke {
            using type = decltype(decl_invoke0<Functor, Args...>());
        };

        template <typename Iteratee, typename Collection>
        using ret_invoke_collection_value_type_t = typename ret_invoke<Iteratee, typename array_value_type<Collection>::type>::type;

        template <class Collection, typename Iteratee, typename ValueType>
        using rebind_to_map_collection_iteratee_t = typename rebind_to_map_key_value<remove_cvref_t<Collection>,   _::Private::ret_invoke_collection_value_type_t<Iteratee,remove_cvref_t<Collection>>, ValueType>::type;

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
            equals(Any&& other) {
                (void) other;
                return false;
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
            inline bool equals(Any && any) {
                (void) any;
                return false;
            }            
        };

        /// vic_func( VIC = Value,Index,Collection);
        template <typename Functor, typename Collection>
        struct is_vic_func_invokable {
            enum {
                value = is_invokable3<Functor,
                typename std::remove_reference<Collection>::type::value_type,
                typename std::remove_reference<Collection>::type::size_type,
                Collection>::value
            };
        };

#ifdef QT_CORE_LIB

        template <class NewType>
        struct rebind<QStringList, NewType> {
            typedef QList<NewType> type;
        };

        template <class NewType>
        struct rebind<QVariantList, NewType> {
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
    } /* End of Private Session */

    template <typename K, typename V, template <typename...> class Map, typename Functor>
    inline Map<K,V>& forIn(Map<K,V> & object, Functor iteratee) {

        static_assert(Private::is_invokable3<Functor, V, K, Map<K,V>>::value, "_::forIn: " UNDERLINE_ITERATEE_MISMATCHED_ERROR);

        Private::Value<typename Private::ret_invoke<Functor, V, K, Map<K,V>>::type> value;

        auto iter = object.begin();
        while (iter != object.end()) {
            value.invoke(iteratee, iter.value(), iter.key(), object);

            if (value.template canConvert<bool>() && value.equals(false)) {
                break;
            }
            iter++;
        }

        return object;
    }

    template <typename K, typename V, template <typename...> class Map, typename Functor>
    inline const Map<K,V>& forIn(const Map<K,V> & object, Functor iteratee) {

        static_assert(Private::is_invokable3<Functor, V, K, Map<K,V>>::value, "_::forIn: " UNDERLINE_ITERATEE_MISMATCHED_ERROR);

        Private::Value<typename Private::ret_invoke<Functor, V, K, Map<K,V>>::type> value;

        auto iter = object.begin();
        while (iter != object.end()) {
            value.invoke(iteratee, iter.value(), iter.key(), object);

            if (value.template canConvert<bool>() && value.equals(false)) {
                break;
            }
            iter++;
        }

        return object;
    }

    template <typename Collection, typename Iteratee>
    inline const Collection& forEach(const Collection& collection, Iteratee iteratee) {
        static_assert(Private::is_vic_func_invokable<Iteratee, Collection>::value, "_::forEach(): " UNDERLINE_ITERATEE_MISMATCHED_ERROR);

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
    template <typename Functor>
    inline QObject* forIn(QObject* object, Functor iteratee) {
        const QMetaObject* meta = object->metaObject();
        static_assert(Private::is_invokable3<Functor, QVariant, QString, QObject*>::value, "_::forIn: " UNDERLINE_ITERATEE_MISMATCHED_ERROR);
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

    template <typename Functor>
    inline const QObject* forIn(const QObject* object, Functor iteratee) {
        const QMetaObject* meta = object->metaObject();
        static_assert(Private::is_invokable3<Functor, QVariant, QString, QObject*>::value, "_::forIn: " UNDERLINE_ITERATEE_MISMATCHED_ERROR);
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

    /*
     If a property contains QObject pointer, it will be converted to QVariantMap.

     In case you need to obtain a QObject pointer, please use get().
     */

    /// Assign properties from source object to the destination object.
    inline QVariantMap& assign(QVariantMap &dest, const QObject *source)
    {
        forIn(source, [&](QVariant value, QString key) {

            if (value.canConvert<QObject*>()) {
                QVariantMap map;
                assign(map, value.value<QObject*>()); // nested properties are not supported yet
                value = map;
            }

            dest[key] = value;
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
        QMap<QString,QVariant>::const_iterator iter = source.begin();
        while (iter != source.end()) {
            dest[iter.key()] = iter.value();
            iter++;
        }
        return dest;
    }

    inline QObject* assign(QObject* dest, const QObject* source) {
        const QMetaObject* sourceMeta = source->metaObject();

        for (int i = 0 ; i < sourceMeta->propertyCount(); i++) {
            const QMetaProperty property = sourceMeta->property(i);
            QString p = property.name();

            QVariant value = source->property(property.name());
            dest->setProperty(p.toLocal8Bit().constData(), value);
        }
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

        static_assert(Private::is_vic_func_invokable<Predicate, Collection>::value, "_::some(): " UNDERLINE_PREDICATE_MISMATCHED_ERROR);
        static_assert(std::is_same<typename Private::ret_invoke<Predicate, typename Private::array_value_type<Collection>::type,int, Collection>::type,bool>::value,
                      "_::some(): " UNDERLINE_PREDICATE_RETURN_TYPE_MISMATCH_ERROR);

        for (unsigned int i = 0 ; i < (unsigned int) collection.size() ; i++) {
            if (Private::invoke(predicate, collection[i], i, collection)) {
                res = true;
                break;
            }
        }
        return res;
    }

    template <typename Collection, typename Iteratee>
    inline auto map(const Collection& collection, Iteratee iteratee) -> typename Private::rebind<Collection,
        typename Private::ret_invoke<Iteratee, typename Private::array_value_type<Collection>::type, int, Collection>::type
    >::type {

        UNDERLINE_STATIC_ASSERT_IS_ARRAY("_::map", Collection);

        static_assert(Private::is_vic_func_invokable<Iteratee, Collection>::value, "_::map(): " UNDERLINE_ITERATEE_MISMATCHED_ERROR);

        typename Private::rebind<Collection, typename Private::ret_invoke<Iteratee, typename Private::array_value_type<Collection>::type, int, Collection>::type>::type res;

        res.reserve((int) collection.size());

        for (unsigned int i = 0 ; i < (unsigned int) collection.size() ; i++) {
            res.push_back(Private::invoke(iteratee, collection[i], i, collection));
        }

        return res;
    }

    template <typename Collection,  typename Iteratee>
    inline auto countBy(const Collection& collection, Iteratee iteratee) -> typename Private::rebind_to_map_collection_iteratee_t<Collection, Iteratee, int>  {

        UNDERLINE_STATIC_ASSERT_IS_ARRAY("_::countBy", Collection);

        static_assert(Private::is_invokable1<Iteratee, typename _::Private::array_value_type<Collection>::type>::value, "_::countBy(): " UNDERLINE_ITERATEE_MISMATCHED_ERROR);

        typename Private::rebind_to_map_collection_iteratee_t<Collection, Iteratee, int>  res;

        for (unsigned int i = 0 ; i < (unsigned int) collection.size() ; i++) {
            auto key = Private::invoke(iteratee, collection[i]);
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
                                             int, Collection>::value, "_::reduce(): " UNDERLINE_ITERATEE_MISMATCHED_ERROR);

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

#ifdef QT_CORE_LIB
    template <typename ...Args>
    QList<int> range_q(Args ...args) {
        return range<QList<int>>(args...);
    }
#endif
}

/* Type Registration */

UL_REGISTER_REBIND_TO_MAP(std::list, std::map)
UL_REGISTER_REBIND_TO_MAP(std::vector, std::map)

#ifdef QT_CORE_LIB
UL_REGISTER_REBIND_TO_MAP(QVector, QMap)
UL_REGISTER_REBIND_TO_MAP(QList, QMap)
#endif

/* End of Type Registration */
