#pragma once

#include "PtrUtils.h"

template <class T>
class UnknownPtr
{
private:
    T* ptr;
    Ptr<T> sptr;
    bool is_raw;
public:
    UnknownPtr(T* val) : ptr(val), is_raw(true) {}
    UnknownPtr(Ptr<T> val) : sptr(val), is_raw(false) {}

    T* Get() { return is_raw ? ptr : sptr.get(); }
    const T* Get() const { return is_raw ? ptr : sptr.get(); }

    T& operator*()
    {
        if (is_raw)
            return *ptr;
        return *sptr;
    }

    const T& operator*() const
    {
        if (is_raw)
            return *ptr;
        return *sptr;
    }

    T* operator->() { return Get(); }
    const T* operator->() const { return Get(); }
};

template <class T>
bool operator== (UnknownPtr<T>& lhs, std::nullptr_t rhs)
{
    return lhs.Get() == rhs;
}

template <class T>
bool operator== (UnknownPtr<T>& lhs, UnknownPtr<T>& rhs)
{
    return lhs.Get() == rhs.Get();
}

template <class T>
bool operator!= (UnknownPtr<T>& lhs, UnknownPtr<T>& rhs)
{
    return !(lhs == rhs);
}

template <class T>
bool operator!= (UnknownPtr<T>& lhs, std::nullptr_t rhs)
{
    return !(lhs == rhs);
}



