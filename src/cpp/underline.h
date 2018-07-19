#pragma once
#include <QList>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
#include <QObject>
#include <functional>

namespace _ {

    namespace Private {

        /// Source: https://stackoverflow.com/questions/5052211/changing-value-type-of-a-given-stl-container
        template <class Container, class NewType>
        struct rebind {
        };

        template <class NewType>
        struct rebind<QStringList, NewType> {
            typedef QList<NewType> type;
        };

        template <class NewType>
        struct rebind<QVariantList, NewType> {
            typedef QList<NewType> type;
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

        template <typename Functor, typename ...Args>
        struct ret_func {
            using type = decltype(decl_func0<Functor, Args&&...>());
        };

        template <typename Functor>
        inline auto invoke(Functor functor) -> decltype(std::declval<Functor>()()) {
            return functor();
        }

        template <typename Functor, typename A1>
        inline auto invoke(Functor functor, A1) ->
            typename std::enable_if<is_args_compatible<Functor>::value,
            decltype(decl_func0<Functor>())>::type {
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

        /* invokte(functor,A1,A2,A3) */

        template <typename Functor, typename A1, typename A2, typename A3>
        inline auto invoke(Functor functor, A1 a1, A2 a2, A3 a3) ->
            typename std::enable_if<is_args_compatible<Functor,A1,A2,A3>::value,
            typename ret_func<Functor,A1, A2,A3>::type>::type {
            return functor(a1,a2,a3);
        }

        template <typename Functor, typename A1, typename A2, typename A3>
        inline auto invoke(Functor functor, A1 a1, A2 a2, A3 ) ->
            typename std::enable_if<is_args_compatible<Functor,A1,A2>::value,
            typename ret_func<Functor,A1, A2>::type>::type {
            return functor(a1,a2);
        }

        template <typename Functor, typename A1, typename A2, typename A3>
        inline auto invoke(Functor functor, A1 a1, A2, A3 ) ->
            typename std::enable_if<is_args_compatible<Functor,A1>::value,
            typename ret_func<Functor,A1>::type>::type {
            return functor(a1);
        }

        template <typename Functor, typename A1, typename A2, typename A3>
        inline auto invoke(Functor functor, A1, A2, A3 ) ->
            typename std::enable_if<is_args_compatible<Functor>::value,
            typename ret_func<Functor>::type>::type {
            return functor();
        }

        /// Declare a function with same output as invoke but taking zero argument.

        template <typename Functor>
        inline decltype(invoke<Functor>(std::declval<Functor>())) decl_invoke0();

        template <typename Functor, typename Arg1>
        inline decltype(invoke<Functor>(std::declval<Functor>(), std::declval<Arg1>())) decl_invoke0();

        template <typename Functor, typename Arg1, typename Arg2>
        inline decltype(invoke<Functor>(std::declval<Functor>(), std::declval<Arg1>(), std::declval<Arg2>())) decl_invoke0();

        template <typename Functor, typename ...Args>
        struct ret_invoke {
            using type = decltype(decl_invoke0<Functor, Args&&...>());
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
        typename Private::ret_invoke<F, typename Private::container_value_type<T>::type, int>::type
    >::type {

        typename Private::rebind<T, typename Private::ret_invoke<F, typename Private::container_value_type<T>::type, int>::type>::type res;

        for (int i = 0 ; i < list.size() ; i++) {
            res << Private::invoke(callback, list[i], i);
        }

        return res;
    }

    /// Gets the value at path of object. If the path is not found, the defaultValue is returned.
    /*
     Example:

     get(object, "a.b.c");

    */


    /// Sets the value at path of object. If a portion of path doesn't exist, it's created.
    /*
     Example:

     set(data, "a.b", 3); // data["a"] will be a QVariantMap that contains a key of "b".

     */



    /*
     If a property contains QObject pointer, it will be converted to QVariantMap.

     In case you need to obtain a QObject pointer, please use get().
     */

    /// Assign properties from source object to the destination object.
    inline void assign(QVariantMap &dest, const QObject *source)
    {
        const QMetaObject* meta = source->metaObject();

        for (int i = 0 ; i < meta->propertyCount(); i++) {
            const QMetaProperty property = meta->property(i);
            QString p = property.name();

            QVariant value = source->property(property.name());

            if (value.canConvert<QObject*>()) {
                QVariantMap map;
                assign(map, value.value<QObject*>()); // nested properties is not supported yet
                value = map;
            }

            dest[p] = value;
        }

    }

    inline void assign(QObject *dest, const QVariantMap & source)
    {
        const QMetaObject* meta = dest->metaObject();

        QMap<QString,QVariant>::const_iterator iter = source.begin();
        while (iter != source.end()) {
            QByteArray key = iter.key().toLocal8Bit();

            int index = meta->indexOfProperty(key.constData());
            if (index < 0) {
                qWarning() << QString("assign:assign a non-existed property: %1").arg(iter.key());
                iter++;
                continue;
            }

            QVariant orig = dest->property(key.constData());
            QVariant value = source[iter.key()];

            if (orig.canConvert<QObject*>()) {
                if (value.type() != QVariant::Map) {
                    qWarning() << QString("assign:expect a QVariantMap property but it is not: %1");
                } else {
                    assign(orig.value<QObject*>(), value.toMap());
                }

            } else if (orig != value) {
                dest->setProperty(key.constData(), value);
            }

            iter++;
        }
    }

    inline void assign(QVariantMap& dest, const QVariantMap& source) {
        QMap<QString,QVariant>::const_iterator iter = source.begin();
        while (iter != source.end()) {
            dest[iter.key()] = iter.value();
            iter++;
        }
    }

    inline void assign(QObject* dest, const QObject* source) {
        const QMetaObject* sourceMeta = source->metaObject();

        for (int i = 0 ; i < sourceMeta->propertyCount(); i++) {
            const QMetaProperty property = sourceMeta->property(i);
            QString p = property.name();

            QVariant value = source->property(property.name());
            dest->setProperty(p.toLocal8Bit().constData(), value);
        }
    }

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

}
