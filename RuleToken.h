#pragma once

#include <functional>

#include "IRuleBuilder.h"
#include "message_exception.h"

namespace Spark
{
    DECL_EXCEPTION(RuleTokenException);

    using RuleFuncWrapper = std::function<void(IRuleBuilder&)>;

    class RuleToken
    {
    private:
        union
        {
            char charVal;
            RuleFuncWrapper funcVal;
        };
        bool isFunc;
        bool isAssigned;

    public:
        RuleToken();

        // Big 5
        RuleToken(const RuleToken& other);
        RuleToken(RuleToken&& other);
        RuleToken& operator=(const RuleToken& other);
        RuleToken& operator=(RuleToken&& other);
        ~RuleToken();

        // Setters
        void Set(char c);
        void Set(RuleFuncWrapper func);

        template <class T>
        void Set(T val)
        {
            Set(RuleFuncWrapper(val));
        }

        // Getters
        inline bool IsFunction() const
        {
            if (!isAssigned)
                throw RuleTokenException("Token is uninitialized.");
            return isFunc;
        }

        char GetChar() const;
        RuleFuncWrapper GetFunc() const;
    };
}
