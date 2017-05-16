#pragma once

#include <memory>

template <class T>
using Ptr = std::shared_ptr<T>;

template <class T, class U>
Ptr<T> PtrCast(Ptr<U> val)
{
    return std::static_pointer_cast<T>(val);
}



