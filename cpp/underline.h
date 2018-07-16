#pragma once
#include <QList>
#include <functional>

namespace _ {

    template <typename T, typename P>
    typename std::enable_if<std::is_convertible<P,std::function<bool(typename std::remove_reference<T>::type::value_type,int)>>::value, bool>::type
    inline some(T& list, P predicate) {
        bool res = false;

        for (int i = 0 ; i < list.size() ; i++) {
            if (predicate(list[i], i)) {
                res = true;
                break;
            }
        }
        return res;
    }

    template <typename T, typename P>
    typename std::enable_if<std::is_convertible<P,std::function<bool(typename std::remove_reference<T>::type::value_type)>>::value, bool>::type
    inline some(T& list, P predicate) {
        bool res = false;

        for (int i = 0 ; i < list.size() ; i++) {
            if (predicate(list[i])) {
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
