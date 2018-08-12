#ifndef SINGLETON_H
#define SINGLETON_H

template <typename T> class Singleton
{
protected:
    // Constructor /Destructor
    Singleton(){}
    virtual ~Singleton(){}

public:
    Singleton(const Singleton &other) = delete;
    Singleton(const Singleton &&other) = delete;
    Singleton & operator=(const Singleton &other) = delete;
    Singleton & operator=(const Singleton &&other) = delete;

    // Public Interface
    static T *getInstance(){
        if (!_singleton) {
            _singleton = new T;
        }
        return (static_cast<T*> (_singleton));
    }

    static void kill()
    {
        if (_singleton)
        {
            delete _singleton;
            _singleton = nullptr;
        }
    }

private:
    // Unique instance
    static T *_singleton;
};

template <typename T> T *Singleton<T>::_singleton;

#endif // SINGLETON_H
