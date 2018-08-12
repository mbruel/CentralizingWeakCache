#include "SharedObject.h"
#include "CentralizingWeakCache.h"


CentralizingWeakCache *SharedObject::sCentralizingCache = CentralizingWeakCache::getInstance();

SharedObject::SharedObject(const QMap<ushort, double> &content)
    :_mass(0.), _content(content), _key()
{
    updateMass();
}

SharedObject::~SharedObject()
{
#ifdef __DEBUG_TRACE__
    qDebug() << "[SharedObject::~SharedObject] destroying " << str();
#endif
    if (!_key.isNull())
        sCentralizingCache->remove(_key);
}

QSharedPointer<SharedObject> SharedObject::clone()
{
    return QSharedPointer<SharedObject>(new SharedObject(_content));
}

#include <cmath>
bool SharedObject::operator==(const SharedObject &other)
{
    if (_mass != other._mass)
        return false;

    if (_content.keys() != other._content.keys())
        return false;

    for (auto it = _content.cbegin(), itEnd = _content.cend(),
         itOther = other._content.cbegin();
         it != itEnd; ++it, ++itOther)
    {
        if (std::fabs(it.value() - itOther.value()) > 1e-5)
            return false;
    }
    return true;
}

void SharedObject::setCacheKey(const QSharedPointer<WeakCacheKey> &key)
{    _key   = key;
}

void SharedObject::updateMass()
{
    for (double mass : _content.values())
        _mass += mass;
}

QString SharedObject::str() const
{
    QString str = QString("mass: %1 (hasKey: %2) content: ").arg(
                _mass).arg(!_key.isNull());
    for (auto it = _content.cbegin(), itEnd = _content.cend() ; it != itEnd; ++it)
        str += QString(" <%1 : %2>").arg(it.key()).arg(it.value());
    return str;
}


void SharedObject::setContent(ushort property, double value)
{
    auto it = _content.find(property);
    if (it != _content.end() )
    {
        if (it.value() != value)
        {
            it.value() = value;
            updateMass();
        }
    }
    else
    {
        _content.insert(property, value);
        updateMass();
    }
}
