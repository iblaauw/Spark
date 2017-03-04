#pragma once

#include "RuleToken.h"

namespace Spark
{
    class IRuleBuilder
    {
    public:
        template <class T, class... Rest>
        inline void Add(T t, Rest... rest)
        {
            RuleToken tok;
            tok.Set(t);
            AddInternal(tok);

            Add(rest...);
        }


        template <class T, class... Rest>
        inline void AddOption(T t, Rest... rest)
        {
            RuleToken tok;
            tok.Set(t);
            AddOptionInternal(tok);

            AddOption(rest...);
        }

        inline void AddEmptyOption()
        {
            AddEmptyOptionInternal();
        }

    private:
        inline void Add() {} // End point
        inline void AddOption() { EndOptionInternal(); } // End point

    protected:
        virtual void AddInternal(RuleToken tok) = 0;
        virtual void AddOptionInternal(RuleToken tok) = 0;
        virtual void EndOptionInternal() = 0;
        virtual void AddEmptyOptionInternal() = 0;
    };

}
