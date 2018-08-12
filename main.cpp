#include <QCoreApplication>

#include "CentralizingWeakCache.h"
#include "SharedObject.h"
#include <QDebug>

static  ushort                nbDistinctElem = 10;
static  CentralizingWeakCache *cache = CentralizingWeakCache::getInstance();

class MainObject{
public:
    MainObject(SharedObject *sharedObj):_sharedPtr(cache->getCentralizedValue(ObjPtr(sharedObj))) {}

private:
    ObjPtr _sharedPtr;
};

void test1()
{
    cache->setSizeMaxOfObsoleteStack(3);

    QVector<QSharedPointer<MainObject>> mainObjects(nbDistinctElem);
    for (ushort i = 0 ; i < nbDistinctElem ; i++)
    {
        mainObjects[i] = QSharedPointer<MainObject>(
                    new MainObject(
                        new SharedObject({ {i, i}, {i+1, i+1}, {i+2, i+2} })));
    }

    qDebug() << "[main] cache size after inserting first " << nbDistinctElem << " distinct instances: " << cache->size();
    cache->dump();
    Q_ASSERT(cache->size() == nbDistinctElem);

    QVector<QSharedPointer<MainObject>> mainObjectsBis(nbDistinctElem);
    for (ushort i = 0 ; i < nbDistinctElem ; i++)
        mainObjectsBis[i] = QSharedPointer<MainObject>(
                    new MainObject(
                        new SharedObject({ {i, i}, {i+1, i+1}, {i+2, i+2} })));

    qDebug() << "[main] cache size after inserting clones : " << cache->size();
    cache->dump();
    Q_ASSERT(cache->size() == nbDistinctElem);

    for (ushort i = 0 ; i < 7 ; i++)
        mainObjectsBis.pop_front();

    qDebug() << "[main] cache size after removing first 7 clones : " << cache->size();
    Q_ASSERT(cache->size() == 10);


    mainObjects.clear();
    qDebug() << "[main] cache size after deleting all first mainObjects : " << cache->size();
    Q_ASSERT(cache->size() == 4); // 3 should be left in the Cache with one in the ObsoleteKey


    qDebug() << "\n\nTEST 1 OK \\o/\n\n";
}

void dumpElements(const QVector<ObjPtr> &elems, const char * vectorName)
{
#ifdef __DEBUG_TRACE__
    qDebug() << "Dumping elements of " << vectorName;
    for (auto it = elems.cbegin(); it != elems.cend() ; ++it)
        qDebug() << "\t - " << (*it)->str();
    CentralizingWeakCache::getInstance()->dump();
#endif
}


#include <QElapsedTimer>

void test2(uint nbObjects)
{

    qDebug() << "\n\ntest2 with " << nbObjects << " (items in cache: " << cache->size() << ")";

    cache->setSizeMaxOfObsoleteStack(1);

    QElapsedTimer cacheTimer;
    cacheTimer.start();


    QVector<ObjPtr> firstObjects;
    firstObjects.reserve(nbObjects);
    for (uint i = 0 ; i < nbObjects ; ++i)
    {
        firstObjects.append(ObjPtr(new SharedObject({ {i, i}, {i+1, i+1}, {i+2, i+2}})));
    }

    dumpElements(firstObjects, "firstObjects");

    qDebug() << "Let's make them unique >>>>>>>>>>>>>>";
    for (uint i = 0 ; i < nbObjects ; ++i)
    {
        ObjPtr &obj = firstObjects[i];
#ifdef __DEBUG_TRACE__
        qDebug() << "Make unique elem #" << i << ": " << obj->str();
#endif
        firstObjects[i] = cache->getCentralizedValue(obj);
    }
    qDebug() << "Let's make them unique <<<<<<<<<<<<<<<";
    dumpElements(firstObjects, "firstObjects");


    qDebug() << "Do some shared copies >>>>>>>>>>>>>>";
    QVector<ObjPtr> sharedObjects;
    sharedObjects.reserve(nbObjects);
    for (uint i = 0 ; i < nbObjects ; ++i)
    {
        sharedObjects.append(cache->getCentralizedValue(firstObjects[i]));
    }
    qDebug() << "Do some shared copies <<<<<<<<<<<<<<<";
    dumpElements(sharedObjects, "sharedObjects");



    ObjPtr &detachedObject = firstObjects[nbObjects-1];
    firstObjects[nbObjects-1] = detachedObject->clone();
    detachedObject->setContent(42, 42);
    dumpElements(firstObjects, "\n\nDetaching last firstObjects");

    firstObjects[nbObjects-1] = cache->getCentralizedValue(firstObjects[nbObjects-1]);
    dumpElements(firstObjects, "\nMaking unique detached object");

    qDebug() << "Destroying last half of firstObjects >>>>>>>>>>>>>>";
    for (uint i = 0 ; i < nbObjects/2 ; ++i)
        firstObjects.pop_back();
    qDebug() << "Destroying last half of firstObjects <<<<<<<<<<<<<<<";
    dumpElements(firstObjects, "");

    qDebug() << "Destroying all shared copies >>>>>>>>>>>>>>";
    sharedObjects.clear();
    qDebug() << "Destroying all shared copies <<<<<<<<<<<<<<<";
#ifdef __DEBUG_TRACE__
    cache->dump();
#endif



    qDebug() << "\nExecution Time using CentralizingWeakCache with " << nbObjects << " items: " << cacheTimer.elapsed();

}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    test1();


    test2(500);
    test2(5000);
    test2(50000);
    test2(500000);
    test2(5000000);


    return a.exec();
}
