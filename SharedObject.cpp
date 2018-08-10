#include "SharedObject.h"
#include "CentralizingWeakCache.h"

SharedObject::SharedObject(const QString &val)
    :_value(val), _cache(nullptr), _key() {}

SharedObject::~SharedObject()
{
    qDebug() << "[SharedObject::~SharedObject] destroying " << _value;
    if (_cache)
        _cache->remove(_key);
}

bool SharedObject::operator==(const SharedObject &other)
{
    return _value == other._value;
}

void SharedObject::setCacheKey(CentralizingWeakCache *cache, const QSharedPointer<WeakCacheKey> &key)
{
    _cache = cache;
    _key   = key;
}
