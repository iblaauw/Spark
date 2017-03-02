#pragma once

#include "RuleFunc.h"
#include "message_exception.h"

namespace Spark
{
    class IRuleBuilder
    {
    public:
        virtual void Add(RuleFunc func) = 0;
        virtual void Add(char c) = 0;

        template <class T, class... Rest>
        void Add(T val, Rest... rest)
        {
            Add(val);
            Add(rest...);
        }

        inline void Add(std::string str)
        {
            for (char c : str)
            {
                Add(c);
            }
        }

        inline void Add(const char* str)
        {
            Add(std::string(str));
        }

        template <class T, class... Rest>
        inline void AddOption(T val, Rest... rest)
        {
            AddOptionInternal(val, rest...);
            if (sizeof...(Rest) == 0)
            {
                EndOptionInternal();
            }
        }

    protected:
        virtual void AddOptionInternal(RuleFunc func) = 0;
        virtual void AddOptionInternal(char c) = 0;
        virtual void EndOptionInternal() = 0;

        template <class T, class... Rest>
        void AddOptionInternal(T val, Rest... rest)
        {
            AddOptionInternal(val);
            AddOptionInternal(rest...);
            if (sizeof...(Rest) == 1)
            {
                EndOptionInternal();
            }
        }

        inline void AddOptionInternal(std::string str)
        {
            for (char c : str)
            {
                AddOptionInternal(c);
            }
        }

        inline void AddOptionInternal(const char* cstr)
        {
            AddOptionInternal(std::string(cstr));
        }
    };

    // TODO: move to own file
    DECL_EXCEPTION(ParseException);
    DECL_EXCEPTION(SparkException);
    DECL_EXCEPTION(SparkAssertionException);
}
