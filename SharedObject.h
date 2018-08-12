#ifndef SHAREDOBJECT_H
#define SHAREDOBJECT_H

//#define __DEBUG_TRACE__ 1

#include <QSharedPointer>
#include <QDebug>

class WeakCacheKey;
class CentralizingWeakCache;

class SharedObject
{
public:
    friend uint qHash(const SharedObject &  cacheKey);
    friend class CentralizingWeakCache; // to set the cache and the key

    explicit SharedObject(const QMap<ushort, double> &content = QMap<ushort, double>());
    SharedObject(const SharedObject&) = delete;
    virtual ~SharedObject();

    QSharedPointer<SharedObject> clone();
    void setContent(ushort property, double value);


    virtual bool operator==(const SharedObject &other);

    QString str() const;


private:
    //used by the CentralizingWeakCache
    void setCacheKey(const QSharedPointer<WeakCacheKey> &key);
    void updateMass();


private:
    double               _mass;
    QMap<ushort, double> _content;


    QSharedPointer<WeakCacheKey> _key;

    static CentralizingWeakCache *sCentralizingCache;
};



#endif // SHAREDOBJECT_H
