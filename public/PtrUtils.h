#pragma once

#include <memory>
#include <functional>
#include <vector>

template <class T>
using Ptr = std::shared_ptr<T>;

template <class T, class U>
Ptr<T> PtrCast(Ptr<U> val)
{
    return std::static_pointer_cast<T>(val);
}

template <class IterT>
class Range
{
private:
    IterT start, finish;
public:
    Range(IterT start, IterT finish) : start(start), finish(finish) {}
    IterT begin() const { return start; }
    IterT end() const { return end; }
};

template <class T, class U, class V>
void Map(V func, T& list, std::vector<U>& vecOut)
{
    vecOut.clear();
    for (auto val : list)
    {
        vecOut.push_back(func(val));
    }
}

template <class U, class T>
void Filter(U func, std::vector<T>& vec)
{
    std::vector<T> temp;
    temp.swap(vec);

    for (auto val : temp)
    {
        if (func(val))
        {
            vec.push_back(val);
        }
    }
}

template <class T>
class Singleton
{
protected:
    Singleton() {}
    Singleton(const Singleton<T>& other) = delete;
    Singleton(Singleton<T>&& other) = delete;
    Singleton<T>& operator= (const Singleton<T>& other) = delete;
    Singleton<T>& operator= (Singleton<T>&& other) = delete;
public:
    static T& Instance()
    {
        static T instance;
        return instance;
    }
};

