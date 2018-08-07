#ifndef WEAKCACHEKEY_H
#define WEAKCACHEKEY_H

#include <QWeakPointer>
#include <QHash>

#include "SharedObject.h"
#include <QSharedPointer>

class WeakCacheKey
{
public:
    friend uint qHash( WeakCacheKey * cacheKey);
    friend bool operator ==(const QSharedPointer<WeakCacheKey> &left, const QSharedPointer<WeakCacheKey> &right);

    inline explicit WeakCacheKey(const QSharedPointer<SharedObject> &sharedPtr);
    virtual ~WeakCacheKey() = default;

private:
    QWeakPointer<SharedObject> _weakPtr;
    uint                       _hashCode;
};


WeakCacheKey::WeakCacheKey(const QSharedPointer<SharedObject> &sharedPtr)
    :_weakPtr(sharedPtr.toWeakRef()), _hashCode()
{}



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


#endif // WEAKCACHEKEY_H
