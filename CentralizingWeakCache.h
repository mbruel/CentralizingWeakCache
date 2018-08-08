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
    if (!cacheKey->_hashComputed && !cacheKey->_weakPtr.isNull())
    {
        QSharedPointer<SharedObject> sharedPtr = cacheKey->_weakPtr.toStrongRef();
        if (!sharedPtr.isNull())
            cacheKey->setComputedHashCode(qHash(*(sharedPtr.data()))); // save the hashCode
    }
    return cacheKey->_hashCode;
}

inline bool operator ==(const QSharedPointer<WeakCacheKey> &left, const QSharedPointer<WeakCacheKey> &right)
{
    // check if weakPtrs are null (same state)
    bool isNull = left->_weakPtr.isNull(), otherIsNull = right->_weakPtr.isNull();
    if ( (isNull && !otherIsNull) || (!isNull && otherIsNull))
        return false;
    if (isNull && otherIsNull)
        return true;

    // both weakPtrs are not null, lets get sharedPtrs
    QSharedPointer<SharedObject> ptr = left->_weakPtr.toStrongRef(),
            otherPtr =   right->_weakPtr.toStrongRef();
    isNull = ptr.isNull(), otherIsNull = otherPtr.isNull();
    if ( (isNull && !otherIsNull) || (!isNull && otherIsNull))
        return false;
    if (isNull && otherIsNull)
        return true;

    // Both sharedPtrs are not null, compare the values of the objects
    return *ptr == *otherPtr;
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
