#pragma once

#include <memory>
#include <functional>

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

