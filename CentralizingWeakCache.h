#ifndef CENTRALIZINGWEAKCACHE_H
#define CENTRALIZINGWEAKCACHE_H

#include <QHash>
#include "SharedObject.h"
#include "WeakCacheKey.h"


inline uint qHash(const SharedObject & sharedObject)
{
    return qHash(sharedObject._value);
}

inline uint qHash(WeakCacheKey * cacheKey)
{
    if (!cacheKey->_weakPtr.isNull())
    {
        QSharedPointer<SharedObject> sharedPtr = cacheKey->_weakPtr.toStrongRef();
        if (!sharedPtr.isNull())
            cacheKey->_hashCode = qHash(*(sharedPtr.data())); // save the hashCode
    }
    return cacheKey->_hashCode;
}

class CentralizingWeakCache : public QObject
{
    Q_OBJECT

public:
    CentralizingWeakCache();
    ~CentralizingWeakCache() = default;

    QSharedPointer<SharedObject> getCentralizedValue(const QSharedPointer<SharedObject> &sharedPtr);

    int size() const;

public slots:
    void handleSharedObjectDestruction(QSharedPointer<WeakCacheKey> key);

private:
    QHash<QSharedPointer<WeakCacheKey>, QWeakPointer<SharedObject>> _cache;
};



#endif // CENTRALIZINGWEAKCACHE_H
