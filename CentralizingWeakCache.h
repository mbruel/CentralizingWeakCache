#ifndef CENTRALIZINGWEAKCACHE_H
#define CENTRALIZINGWEAKCACHE_H
#include <QMutexLocker>
#include <QHash>
#include "SharedObject.h"
#include "WeakCacheKey.h"


inline uint qHash(const SharedObject & sharedObject)
{
    return qHash(sharedObject._value);
}

inline uint qHash(WeakCacheKey * cacheKey)
{
    if (!cacheKey->_weakPtr.isNull())
    {
        QSharedPointer<SharedObject> sharedPtr = cacheKey->_weakPtr.toStrongRef();
        if (!sharedPtr.isNull())
            cacheKey->_hashCode = qHash(*(sharedPtr.data())); // save the hashCode
    }
    return cacheKey->_hashCode;
}

class CentralizingWeakCache
{

public:
    CentralizingWeakCache();
    ~CentralizingWeakCache() = default;

    QSharedPointer<SharedObject> getCentralizedValue(const QSharedPointer<SharedObject> &sharedPtr);
    void remove(const QSharedPointer<WeakCacheKey> &key);
    int size() const;

private:
    QHash<QSharedPointer<WeakCacheKey>, QWeakPointer<SharedObject>> _cache;
    mutable QMutex                                                  _mutex;
};



#endif // CENTRALIZINGWEAKCACHE_H
