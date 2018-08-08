#include <QCoreApplication>

#include "CentralizingWeakCache.h"
#include "SharedObject.h"

static  CentralizingWeakCache cache;

class MainObject{
public:
    MainObject(SharedObject *sharedObj):_sharedPtr(cache.getCentralizedValue(QSharedPointer<SharedObject>(sharedObj))) {}

private:
    QSharedPointer<SharedObject> _sharedPtr;
};


#include <QDebug>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MainObject *obj1 = new MainObject(new SharedObject("Object 1")),
            *obj1bis = new MainObject(new SharedObject("Object 1"));

    qDebug() << "[main] cache size after inserting two same instance: " << cache.size();

    delete obj1;

    qDebug() << "[main] cache size after deleting 1 instance: " << cache.size();

    delete obj1bis;

    qDebug() << "[main] cache size after deleting both instance: " << cache.size();

    return a.exec();
}
