#ifndef SHAREDOBJECT_H
#define SHAREDOBJECT_H

#include <QObject>
#include <QSharedPointer>
#include <QDebug>

class WeakCacheKey;

class SharedObject : public QObject
{
    Q_OBJECT

public:
    friend uint qHash(const SharedObject &  cacheKey);
    friend class CentralizingWeakCache;

    explicit SharedObject(const QString &val)
        :_value(val), _key() {}

    virtual ~SharedObject()
    {
        qDebug() << "[SharedObject::~SharedObject] destroying " << _value;
        if (!_key.isNull())
            emit destroyed(_key);
    }

    virtual bool operator==(const SharedObject &other) {return _value == other._value;}

signals:
    void destroyed(QSharedPointer<WeakCacheKey> key);

private:
    QString _value;

    QSharedPointer<WeakCacheKey> _key;
};



#endif // SHAREDOBJECT_H
