#include <QCoreApplication>

#include "CentralizingWeakCache.h"
#include "SharedObject.h"
#include <QDebug>

static  ushort                nbDistinctElem = 10;
static  CentralizingWeakCache cache(3);

class MainObject{
public:
    MainObject(SharedObject *sharedObj):_sharedPtr(cache.getCentralizedValue(QSharedPointer<SharedObject>(sharedObj))) {}

private:
    QSharedPointer<SharedObject> _sharedPtr;
};


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QVector<QSharedPointer<MainObject>> mainObjects(nbDistinctElem);
    for (ushort i = 0 ; i < nbDistinctElem ; i++)
        mainObjects[i] = QSharedPointer<MainObject>(new MainObject(new SharedObject(QString("Object %1").arg(i))));

    qDebug() << "[main] cache size after inserting first " << nbDistinctElem << " distinct instances: " << cache.size();
    Q_ASSERT(cache.size() == nbDistinctElem);

    QVector<QSharedPointer<MainObject>> mainObjectsBis(nbDistinctElem);
    for (ushort i = 0 ; i < nbDistinctElem ; i++)
        mainObjectsBis[i] = QSharedPointer<MainObject>(new MainObject(new SharedObject(QString("Object %1").arg(i))));

    qDebug() << "[main] cache size after inserting clones : " << cache.size();
    Q_ASSERT(cache.size() == nbDistinctElem);

    for (ushort i = 0 ; i < 7 ; i++)
        mainObjectsBis.pop_front();

    qDebug() << "[main] cache size after removing first 7 clones : " << cache.size();
    Q_ASSERT(cache.size() == 10);


    mainObjects.clear();
    qDebug() << "[main] cache size after deleting all first mainObjects : " << cache.size();
    Q_ASSERT(cache.size() == 4); // 3 should be left in the Cache with one in the ObsoleteKey

    return a.exec();
}
