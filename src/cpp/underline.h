#pragma once
#include <QList>
#include <functional>

namespace _ {

    namespace Private {

        /// Source: https://stackoverflow.com/questions/5052211/changing-value-type-of-a-given-stl-container
        template <class Container, class NewType>
        struct rebind;

        template <class ValueType, class... Args, template <class...> class Container, class NewType>
        struct rebind<Container<ValueType, Args...>, NewType>
        {
            typedef Container<NewType, typename rebind<Args, NewType>::type...> type;
        };

        template <typename Functor, typename A1>
        struct ret_arg1_traits {
            typedef decltype(std::declval<Functor>()(std::declval<A1>())) type;
        };

        template <typename T>
        struct container_value_type {
            typedef typename std::remove_reference<T>::type::value_type type;
        };

        template <typename Functor,typename ...Args>
        struct is_args_compatible {
            enum {
                value = std::is_convertible<Functor, std::function<void(Args...)> >::value
            };
        };

        /// Convert to a function with zero paramter. Only the return type remained the same.
        template <typename Functor>
        decltype(std::declval<Functor>()()) func0();

        template <typename Functor, typename Arg1>
        decltype(std::declval<Functor>()(std::declval<Arg1>()))
        func0();

        template <typename Functor, typename Arg1, typename Arg2>
        decltype(std::declval<Functor>()(std::declval<Arg1>(), std::declval<Arg2>()))
        func0();

        template <typename Functor, typename ...Args>
        struct ret_func {
            using type = decltype(func0<Functor, Args&&...>());
        };

        template <typename Functor>
        inline auto invoke(Functor functor) -> decltype(std::declval<Functor>()()) {
            return functor();
        }

        template <typename Functor, typename A1>
        inline auto invoke(Functor functor, A1 a1) ->
            typename std::enable_if<is_args_compatible<Functor, A1>::value,         decltype(func0<Functor, A1>()) >::type {
            return functor(a1);
        }

        template <typename Functor, typename A1>
        inline auto invoke(Functor functor, A1) ->
            typename std::enable_if<is_args_compatible<Functor>::value,
            decltype(func0<Functor>())>::type {
            return functor();
        }

        /* invoke(Functor,A1,A2) */

        template <typename Functor, typename A1, typename A2>
        inline auto invoke(Functor functor, A1 a1, A2 a2) ->
            typename std::enable_if<is_args_compatible<Functor,A1,A2>::value,
            decltype(func0<Functor,A1,A2>())>::type {
            return functor(a1,a2);
        }

        template <typename Functor, typename A1, typename A2>
        inline auto invoke(Functor functor, A1 a1, A2) ->
            typename std::enable_if<is_args_compatible<Functor,A1>::value,
            decltype(func0<Functor,A1>())>::type {
            return functor(a1);
        }

        template <typename Functor, typename A1, typename A2>
        inline auto invoke(Functor functor, A1, A2) ->
            typename std::enable_if<is_args_compatible<Functor>::value,
            decltype(func0<Functor>())>::type {
            return functor();
        }

    }

    template <typename T, typename P>
    inline bool some(T& list, P predicate) {
        bool res = false;

        for (int i = 0 ; i < list.size() ; i++) {
            if (Private::invoke(predicate, list[i], i)) {
                res = true;
                break;
            }
        }
        return res;
    }

    template <typename T, typename F>
    inline auto map(T& list, F callback) -> QList<
        typename Private::ret_arg1_traits<F, typename Private::container_value_type<T>::type>::type
    > {

        QList<typename Private::ret_arg1_traits<F, typename Private::container_value_type<T>::type>::type> res;

        for (int i = 0 ; i < list.size() ; i++) {
            res << callback(list[i]);
        }

        return res;
    }
}
