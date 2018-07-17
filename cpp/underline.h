#pragma once
#include <QList>
#include <functional>

namespace _ {

    namespace Private {

        /// Source: https://stackoverflow.com/questions/5052211/changing-value-type-of-a-given-stl-container
        template <class Container, class NewType>
        struct rebind {
        };

        template <class ValueType, class... Args, template <class...> class Container, class NewType>
        struct rebind<Container<ValueType, Args...>, NewType>
        {
            typedef Container<NewType, typename rebind<Args, NewType>::type...> type;
        };

        template <typename T>
        struct container_value_type {
            typedef typename std::remove_reference<T>::type::value_type type;
        };

        /// Check is the Functor be able to take Args as input. It works with generic lambda.
        template <typename Functor,typename ...Args>
        struct is_args_compatible {
            enum {
                value = std::is_convertible<Functor, std::function<void(Args...)> >::value
            };
        };

        /// Convert to a function with zero paramter. Only the return type remained the same.
        template <typename Functor>
        typename std::enable_if<is_args_compatible<Functor>::value,
        decltype(std::declval<Functor>()())>::type
        func0();

        template <typename Functor>
        typename std::enable_if<!is_args_compatible<Functor>::value,void>::type
        func0();

        template <typename Functor, typename Arg1>
        typename std::enable_if<is_args_compatible<Functor, Arg1>::value,
        decltype(std::declval<Functor>()(std::declval<Arg1>()))>::type
        func0();

        template <typename Functor, typename Arg1>
        typename std::enable_if<!is_args_compatible<Functor, Arg1>::value, void>::type
        func0();

        template <typename Functor, typename Arg1, typename Arg2>
        typename std::enable_if<is_args_compatible<Functor, Arg1, Arg2>::value,
        decltype(std::declval<Functor>()(std::declval<Arg1>(),std::declval<Arg2>()))>::type
        func0();

        template <typename Functor, typename Arg1, typename Arg2>
        typename std::enable_if<!is_args_compatible<Functor, Arg1, Arg2>::value,void>::type
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
        inline auto invoke(Functor functor, A1) ->
            typename std::enable_if<is_args_compatible<Functor>::value,
            decltype(func0<Functor>())>::type {
            return functor();
        }

        template <typename Functor, typename A1>
        inline auto invoke(Functor functor, A1 a1) ->
        typename std::enable_if<is_args_compatible<Functor,A1>::value,
        typename ret_func<Functor,A1>::type>::type {
            return functor(a1);
        }

        /* invoke(Functor,A1,A2) */

        template <typename Functor, typename A1, typename A2>
        inline auto invoke(Functor functor, A1 a1, A2 a2) ->
            typename std::enable_if<is_args_compatible<Functor,A1,A2>::value,
            typename ret_func<Functor,A1, A2>::type>::type {
            return functor(a1,a2);
        }

        template <typename Functor, typename A1, typename A2>
        inline auto invoke(Functor functor, A1 a1, A2) ->
            typename std::enable_if<is_args_compatible<Functor,A1>::value,
            typename ret_func<Functor,A1>::type>::type {
            return functor(a1);
        }

        template <typename Functor, typename A1, typename A2>
        inline auto invoke(Functor functor, A1, A2) ->
            typename std::enable_if<is_args_compatible<Functor>::value,
            typename ret_func<Functor>::type>::type{
            return functor();
        }

    }

    template <typename T, typename P>
    inline bool some(const T& list, P predicate) {
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
    inline auto map(const T& list, F callback) -> typename Private::rebind<T,
        typename Private::ret_func<F, typename Private::container_value_type<T>::type>::type
    >::type {

        typename Private::rebind<T, typename Private::ret_func<F, typename Private::container_value_type<T>::type>::type>::type res;

        for (int i = 0 ; i < list.size() ; i++) {
            res << callback(list[i]);
        }

        return res;
    }
}
