#include "CentralizingWeakCache.h"

#include <QDebug>

CentralizingWeakCache::CentralizingWeakCache(ushort nbOfObsoleteKeysThatScheduleCleanUp)
    :_cache(), _secureCache(), _obsoleteKeys(new QSharedPointer<WeakCacheKey>[nbOfObsoleteKeysThatScheduleCleanUp]),
      _nextObsoleteKeyIndex(0), _secureOsoleteStack(),
      _sizeMaxOfObsoleteStack(nbOfObsoleteKeysThatScheduleCleanUp)
{
}

CentralizingWeakCache::~CentralizingWeakCache()
{
    delete[] _obsoleteKeys;
}

QSharedPointer<SharedObject> CentralizingWeakCache::getCentralizedValue(const QSharedPointer<SharedObject> &sharedPtr)
{
    QSharedPointer<SharedObject> centralizedValue;
    QSharedPointer<WeakCacheKey> key(new WeakCacheKey(sharedPtr));

    QWriteLocker lockCache(&_secureCache);
    QWeakPointer<SharedObject> cachedWeakPtr = _cache.value(key, QWeakPointer<SharedObject>());
    if (!cachedWeakPtr.isNull())
        centralizedValue = cachedWeakPtr.toStrongRef();

    if (centralizedValue.isNull())
    {
        centralizedValue = sharedPtr;
        centralizedValue->setCacheKey(this, key);
        qDebug() << "[CentralizingWeakCache::getCentralizedValue] adding new value in cache : " << centralizedValue->_value;
        _cache.insert(key, centralizedValue.toWeakRef());                
    }
    else
        qDebug() << "[CentralizingWeakCache::getCentralizedValue] getting centralized value for : " << centralizedValue->_value;

    return centralizedValue;
}

void CentralizingWeakCache::remove(const QSharedPointer<WeakCacheKey> &key)
{
    QWriteLocker lockStack(&_secureOsoleteStack);
    _obsoleteKeys[_nextObsoleteKeyIndex++] = key;
    qDebug() << "[CentralizingWeakCache::handleSharedObjectDestruction] add key in _obsoleteKeys, size: " << _nextObsoleteKeyIndex;

    if (_nextObsoleteKeyIndex == _sizeMaxOfObsoleteStack)
        _cleanUpCache();
}

int CentralizingWeakCache::size() const
{
    QReadLocker lockCache(&_secureCache);
    return _cache.size();
}

void CentralizingWeakCache::_cleanUpCache()
{
    qDebug() << "[CentralizingWeakCache::_cleanUpCache] cleanUp: we've " << _sizeMaxOfObsoleteStack << " obsolete keys...";
    QWriteLocker lockCache(&_secureCache);
    for (ushort idx = 0; idx < _sizeMaxOfObsoleteStack ; ++idx)
         _cache.remove(_obsoleteKeys[idx]);

    _nextObsoleteKeyIndex = 0;
}

