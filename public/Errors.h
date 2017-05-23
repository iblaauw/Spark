#pragma once

#include <iostream>
#include <exception>

/// Helpers

template <class Type1, class... Types>
struct _PrintHelper
{
    static void Print(std::ostream& os, Type1 val, Types... rest)
    {
        os << val;
        _PrintHelper<Types...>::Print(os, rest...);
    }
};

template <class Type1>
struct _PrintHelper<Type1>
{
    static void Print(std::ostream& os, Type1 val)
    {
        os << val;
    }
};

template <class Type1, class... Types>
inline void _DoPrint(std::ostream& os, Type1 val, Types... rest)
{
    _PrintHelper<Type1, Types...>::Print(os, val, rest...);
}



bool HasError();

void SetError();

class AssertionException : public std::exception
{};

template <class T1, class... Types>
void Assert(bool b, T1 arg1, Types... args)
{
    if (!b)
    {
        std::cerr << "Internal Error: ";
        _DoPrint(std::cerr, arg1, args...);
        std::cerr << std::endl;
        SetError();
        throw AssertionException();
    }
}

template <class Type1, class... Types>
void Error(Type1 t1, Types... types)
{
    std::cerr << "Error: ";
    _DoPrint(std::cerr, t1, types...);
    std::cerr << std::endl;
    SetError();
}

