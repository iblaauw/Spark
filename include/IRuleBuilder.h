#pragma once

#include <functional>

#include "RuleToken.h"
#include "Delegates.h"
#include "Node.h"

#define Autoname(builder) \
do \
{ \
    builder.SetName(__func__); \
} \
while(false)


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
            RequestFlatten();
        }

        virtual void SetName(std::string name) = 0;

        template <class T>
        void SetNodeType()
        {
            auto factory = [](std::vector<NodePtr>& vec) {
                std::shared_ptr<T> val = std::make_shared<T>(vec);
                return AsNode(val);
            };
            SetNodeTypeInternal(factory);
        }

        void Ignore(int index)
        {
            IgnoreInternal(index);
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
        virtual void RequestFlatten() = 0;
        virtual void SetNodeTypeInternal(std::function<NodePtr(std::vector<NodePtr>&)> factory) = 0;
        virtual void IgnoreInternal(int index) = 0;
    };

}
