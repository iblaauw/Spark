#pragma once

#include "RuleToken.h"
#include "Delegates.h"

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

        template <class T>
        void AddString(T val)
        {
            auto rule = GenerateStringRule(val);
            Add(rule);
        }

    private:
        inline void Add() { EndInternal(); } // End point

        template <class T>
        RuleFuncWrapper GenerateStringRule(T& val)
        {
            auto end = [=](IRuleBuilder& b)
            {
                auto base = GenerateStringRule(val);
                b.Add(base);
                b.AddEmpty();
            };
            auto base = [=](IRuleBuilder& b) { b.Add(val, end); };
            return base;
        }

    protected:
        virtual void AddInternal(RuleToken tok) = 0;
        virtual void EndInternal() = 0;
        virtual void AddEmptyInternal() = 0;
    };

}
