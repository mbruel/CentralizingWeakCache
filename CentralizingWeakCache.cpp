#include "CentralizingWeakCache.h"

#include <QDebug>

CentralizingWeakCache::CentralizingWeakCache()
    :_cache()
{}

QSharedPointer<SharedObject> CentralizingWeakCache::getCentralizedValue(const QSharedPointer<SharedObject> &sharedPtr)
{
    QSharedPointer<SharedObject> centralizedValue;
    QSharedPointer<WeakCacheKey> key(new WeakCacheKey(sharedPtr));
    QWeakPointer<SharedObject> cachedWeakPtr = _cache.value(key, QWeakPointer<SharedObject>());
    if (!cachedWeakPtr.isNull())
        centralizedValue = cachedWeakPtr.toStrongRef();

    if (centralizedValue.isNull())
    {
        centralizedValue = sharedPtr;
        _cache.insert(key, centralizedValue.toWeakRef());
        centralizedValue->_key = key;
        connect(centralizedValue.data(), &SharedObject::destroyed,
                this, &CentralizingWeakCache::handleSharedObjectDestruction, Qt::DirectConnection);
        qDebug() << "[CentralizingWeakCache::getCentralizedValue] adding new value in cache : " << centralizedValue->_value;
    }
    else
        qDebug() << "[CentralizingWeakCache::getCentralizedValue] getting centralized value for : " << centralizedValue->_value;

    return centralizedValue;
}

int CentralizingWeakCache::size() const
{
    return _cache.size();
}

void CentralizingWeakCache::handleSharedObjectDestruction(QSharedPointer<WeakCacheKey> key)
{
    int res = _cache.remove(key);
    qDebug() << "[CentralizingWeakCache::handleSharedObjectDestruction] removing centralized value: " << res;

}
