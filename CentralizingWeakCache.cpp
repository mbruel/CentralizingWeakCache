#include "CentralizingWeakCache.h"

#include <QDebug>

CentralizingWeakCache::CentralizingWeakCache()
    :_cache(), _mutex()
{}

QSharedPointer<SharedObject> CentralizingWeakCache::getCentralizedValue(const QSharedPointer<SharedObject> &sharedPtr)
{
    QSharedPointer<SharedObject> centralizedValue;
    QSharedPointer<WeakCacheKey> key(new WeakCacheKey(sharedPtr));

    QMutexLocker locker(&_mutex);
    QWeakPointer<SharedObject> cachedWeakPtr = _cache.value(key, QWeakPointer<SharedObject>());
    if (!cachedWeakPtr.isNull())
        centralizedValue = cachedWeakPtr.toStrongRef();

    if (centralizedValue.isNull())
    {
        centralizedValue = sharedPtr;
        _cache.insert(key, centralizedValue.toWeakRef());
        centralizedValue->setCacheKey(this, key);
        qDebug() << "[CentralizingWeakCache::getCentralizedValue] adding new value in cache : " << centralizedValue->_value;
    }
    else
        qDebug() << "[CentralizingWeakCache::getCentralizedValue] getting centralized value for : " << centralizedValue->_value;

    return centralizedValue;
}

void CentralizingWeakCache::remove(const QSharedPointer<WeakCacheKey> &key)
{
    QMutexLocker locker(&_mutex);
    int res = _cache.remove(key);
    qDebug() << "[CentralizingWeakCache::handleSharedObjectDestruction] removing centralized value: " << res;
}

int CentralizingWeakCache::size() const
{
    QMutexLocker locker(&_mutex);
    return _cache.size();
}

