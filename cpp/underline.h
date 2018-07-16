#pragma once
#include <QList>
#include <functional>

namespace _ {

// function_traits: Source: http://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda

    namespace Private {
        template <typename T>
        struct function_traits
                : public function_traits<decltype(&T::operator())>
        {};

        template <typename ClassType, typename ReturnType, typename... Args>
        struct function_traits<ReturnType(ClassType::*)(Args...) const>
        // we specialize for pointers to member function
        {
            enum { arity = sizeof...(Args) };
            // arity is the number of arguments.

            typedef ReturnType result_type;

            template <size_t i>
            struct arg
            {
                typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
                // the i-th argument is equivalent to the i-th tuple element of a tuple
                // composed of those arguments.
            };
        };

        /* It is an additional to the original function_traits to handle non-const function (with mutable keyword lambda). */

        template <typename ClassType, typename ReturnType, typename... Args>
        struct function_traits<ReturnType(ClassType::*)(Args...)>
        // we specialize for pointers to member function
        {
            enum { arity = sizeof...(Args) };
            // arity is the number of arguments.

            typedef ReturnType result_type;

            template <size_t i>
            struct arg
            {
                typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
                // the i-th argument is equivalent to the i-th tuple element of a tuple
                // composed of those arguments.
            };
        };

    }
    // End or Private

    template <typename T, typename P>
    inline bool some(T& list, P predicate) {
        bool res = false;

        for (int i = 0 ; i < list.size() ; i++) {
            // @TODO: Improvement. May use predicate with/without index parameter
            if (predicate(list[i], i)) {
                res = true;
                break;
            }
        }
        return res;
    }

    template <typename T, typename F>
    inline auto map(T& list, F callback) -> QList<
         decltype(callback(std::declval< typename std::remove_reference<T>::type::value_type>()))
    > {

        QList<decltype(callback(std::declval<typename std::remove_reference<T>::type::value_type>()))> res;

        for (int i = 0 ; i < list.size() ; i++) {
            res << callback(list[i]);
        }

        return res;
    }

}
