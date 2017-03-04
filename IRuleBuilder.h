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

        inline void AddEmpty()
        {
            AddEmptyInternal();
        }

        //template <class T>
        //void AddStringOption(T val)
        //{
        //    auto stringEnd = [&](IRuleBuilder& b) { b.AddStringOption(val); b.AddEmptyOption(); };
        //    AddOption(val, stringEnd);
        //}

    private:
        inline void Add() { EndInternal(); } // End point

    protected:
        virtual void AddInternal(RuleToken tok) = 0;
        virtual void EndInternal() = 0;
        virtual void AddEmptyInternal() = 0;
    };

}
