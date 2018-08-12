#include "CentralizingWeakCache.h"

#include <QDebug>

CentralizingWeakCache::CentralizingWeakCache(ushort nbOfObsoleteKeysThatScheduleCleanUp)
    : Singleton<CentralizingWeakCache>(),
      _cache(), _secureCache(), _obsoleteKeys(new KeyPtr[nbOfObsoleteKeysThatScheduleCleanUp]),
      _nextObsoleteKeyIndex(0), _secureOsoleteStack(),
      _sizeMaxOfObsoleteStack(nbOfObsoleteKeysThatScheduleCleanUp)
{
}

CentralizingWeakCache::~CentralizingWeakCache()
{
    delete[] _obsoleteKeys;
}

ObjPtr CentralizingWeakCache::getCentralizedValue(const ObjPtr &sharedPtr)
{
    ObjPtr centralizedValue;
    KeyPtr key(new WeakCacheKey(sharedPtr));

    QWriteLocker lockCache(&_secureCache);
    ObjWeakPtr cachedWeakPtr = _cache.value(key, ObjWeakPtr());
    if (!cachedWeakPtr.isNull())
        centralizedValue = cachedWeakPtr.toStrongRef();

    if (centralizedValue.isNull())
    {
        centralizedValue = sharedPtr;
        centralizedValue->setCacheKey(key);
#ifdef __DEBUG_TRACE__
        qDebug() << "[CentralizingWeakCache::getCentralizedValue] adding new value in cache : " << centralizedValue->str();
#endif
        _cache.insert(key, centralizedValue.toWeakRef());                
    }
#ifdef __DEBUG_TRACE__
    else
        qDebug() << "[CentralizingWeakCache::getCentralizedValue] getting centralized value for : " << centralizedValue->str();
#endif

    return centralizedValue;
}

void CentralizingWeakCache::remove(const KeyPtr &key)
{
    QWriteLocker lockStack(&_secureOsoleteStack);
    _obsoleteKeys[_nextObsoleteKeyIndex++] = key;
#ifdef __DEBUG_TRACE__
    qDebug() << "[CentralizingWeakCache::handleSharedObjectDestruction] add key in _obsoleteKeys, size: " << _nextObsoleteKeyIndex;
#endif
    if (_nextObsoleteKeyIndex == _sizeMaxOfObsoleteStack)
        _cleanUpCache();
}

int CentralizingWeakCache::size() const
{
    QReadLocker lockCache(&_secureCache);
    return _cache.size();
}

void CentralizingWeakCache::dump(const QString &msg)
{
    QReadLocker lockCache(&_secureCache);
    qDebug() << "[CentralizingCache::dump] " << msg << " Size = " << _cache.size();
    auto nb = 0;
    for (auto it = _cache.cbegin(), itEnd = _cache.cend() ; it != itEnd; ++it)
    {
        if (!it->isNull())
        {
            qDebug() << "\t #" << ++nb << ": " << it->toStrongRef()->str();
        }
    }
}

void CentralizingWeakCache::_cleanUpCache()
{
#ifdef __DEBUG_TRACE__
    qDebug() << "[CentralizingWeakCache::_cleanUpCache] cleanUp: we've " << _sizeMaxOfObsoleteStack << " obsolete keys...";
#endif
    QWriteLocker lockCache(&_secureCache);
    for (ushort idx = 0; idx < _sizeMaxOfObsoleteStack ; ++idx)
         _cache.remove(_obsoleteKeys[idx]);

    _nextObsoleteKeyIndex = 0;
}

