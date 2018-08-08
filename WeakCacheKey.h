#ifndef WEAKCACHEKEY_H
#define WEAKCACHEKEY_H

#include <QWeakPointer>
#include <QSharedPointer>
class SharedObject;

class WeakCacheKey
{
public:
    friend uint qHash(WeakCacheKey *cacheKey);
    friend bool operator ==(const QSharedPointer<WeakCacheKey> &left, const QSharedPointer<WeakCacheKey> &right);

    WeakCacheKey() = delete;
    WeakCacheKey(const WeakCacheKey&)  = default;
    inline explicit WeakCacheKey(const QSharedPointer<SharedObject> &sharedPtr);

    virtual ~WeakCacheKey() = default;

private:
    inline void setComputedHashCode(uint hashCode);

private:
    QWeakPointer<SharedObject> _weakPtr;
    uint                       _hashCode;
    bool                       _hashComputed;
};


WeakCacheKey::WeakCacheKey(const QSharedPointer<SharedObject> &sharedPtr)
    :_weakPtr(sharedPtr.toWeakRef()), _hashCode(0), _hashComputed(false)
{}

void WeakCacheKey::setComputedHashCode(uint hashCode)
{
    _hashCode     = hashCode;
    _hashComputed = true;
}

#endif // WEAKCACHEKEY_H
