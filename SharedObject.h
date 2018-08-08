#ifndef SHAREDOBJECT_H
#define SHAREDOBJECT_H

#include <QSharedPointer>
#include <QDebug>

class WeakCacheKey;
class CentralizingWeakCache;

class SharedObject
{
public:
    friend uint qHash(const SharedObject &  cacheKey);
    friend class CentralizingWeakCache; // to set the cache and the key

    explicit SharedObject(const QString &val);

    virtual ~SharedObject();

    virtual bool operator==(const SharedObject &other);

private:
    //used by the CentralizingWeakCache
    void setCacheKey(CentralizingWeakCache *cache, const QSharedPointer<WeakCacheKey> &key);

private:
    QString                      _value;
    CentralizingWeakCache       *_cache;
    QSharedPointer<WeakCacheKey> _key;

};



#endif // SHAREDOBJECT_H
