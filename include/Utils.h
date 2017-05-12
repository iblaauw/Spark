#pragma once

#include <memory>

namespace Spark
{
    template <class... Args>
    struct _Vec_impl2;

    template <class Arg1, class... Args>
    struct _Vec_impl2<Arg1, Args...>
    {
        template <class VType>
        static void Do(std::vector<VType>& vec, Arg1 arg1, Args... args)
        {
            vec.push_back(arg1);
            _Vec_impl2<Args...>::Do(vec, args...);
        }
    };

    template <>
    struct _Vec_impl2<>
    {
        template <class VType>
        static void Do(std::vector<VType>& vec)
        {
             // NOOP
        }
    };

    template <class VType, class... Args>
    std::vector<VType> Vectorify(Args... args)
    {
        std::vector<VType> vec;
        _Vec_impl2<Args...>::Do(vec, args...);
        return vec;
    }


    class Node;
    void PrintTree(std::shared_ptr<Node> root, int level = 0);
}

