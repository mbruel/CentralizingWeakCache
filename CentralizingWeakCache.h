#ifndef CENTRALIZINGWEAKCACHE_H
#define CENTRALIZINGWEAKCACHE_H
#include <QReadWriteLock>
#include <QHash>
#include <QStack>
#include "SharedObject.h"
#include "WeakCacheKey.h"
#include "Singleton.h"

using ObjPtr     = QSharedPointer<SharedObject>;
using ObjWeakPtr = QWeakPointer<SharedObject>;
using KeyPtr     = QSharedPointer<WeakCacheKey>;

inline uint qHash(const SharedObject & sharedObject)
{
    return sharedObject._mass;
}

inline uint qHash(WeakCacheKey * cacheKey)
{
    if (!cacheKey->_hashComputed && !cacheKey->_weakPtr.isNull())
    {
        ObjPtr sharedPtr = cacheKey->_weakPtr.toStrongRef();
        if (!sharedPtr.isNull())
            cacheKey->setComputedHashCode(qHash(*(sharedPtr.data()))); // save the hashCode
    }
    return cacheKey->_hashCode;
}

inline bool operator ==(const KeyPtr &left, const KeyPtr &right)
{
    // check if weakPtrs are null (same state)
    bool isNull = left->_weakPtr.isNull(), otherIsNull = right->_weakPtr.isNull();
    if ( (isNull && !otherIsNull) || (!isNull && otherIsNull))
        return false;
    if (isNull && otherIsNull)
        return true;

    // both weakPtrs are not null, lets get sharedPtrs
    ObjPtr ptr = left->_weakPtr.toStrongRef(),
            otherPtr =   right->_weakPtr.toStrongRef();
    isNull = ptr.isNull(), otherIsNull = otherPtr.isNull();
    if ( (isNull && !otherIsNull) || (!isNull && otherIsNull))
        return false;
    if (isNull && otherIsNull)
        return true;

    // Both sharedPtrs are not null, compare the values of the objects
    return *ptr == *otherPtr;
}

class CentralizingWeakCache : public Singleton<CentralizingWeakCache>
{

public:
    friend class Singleton<CentralizingWeakCache>;

    ~CentralizingWeakCache();

    ObjPtr getCentralizedValue(const ObjPtr &sharedPtr);
    void remove(const KeyPtr &key);
    int size() const;

    void dump(const QString &msg = "");
    inline void setSizeMaxOfObsoleteStack(ushort nb);


private:
    void _cleanUpCache();
    CentralizingWeakCache(ushort nbOfObsoleteKeysThatScheduleCleanUp = sDefaultNumberOfObsoleteKeysThatScheduleCleanUp);


private:
    QHash<KeyPtr, ObjWeakPtr> _cache;
    mutable QReadWriteLock    _secureCache;
    KeyPtr                   *_obsoleteKeys; //!< this will be a C array
    ushort                    _nextObsoleteKeyIndex;
    QReadWriteLock            _secureOsoleteStack;
    ushort                    _sizeMaxOfObsoleteStack;

    static const ushort sDefaultNumberOfObsoleteKeysThatScheduleCleanUp = 1024;
};

void CentralizingWeakCache::setSizeMaxOfObsoleteStack(ushort nb)
{
    _sizeMaxOfObsoleteStack = nb;
}



#endif // CENTRALIZINGWEAKCACHE_H
