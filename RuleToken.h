#pragma once

#include <functional>

#include "Delegates.h"
#include "message_exception.h"

namespace Spark
{
    DECL_EXCEPTION(RuleTokenException);

    class RuleToken
    {
    public:
        enum Mode { CHAR_MODE, STRING_MODE, FUNC_MODE };
    private:
        union
        {
            char charVal;
            std::string stringVal;
            RuleFuncWrapper funcVal;
        };

        Mode mode;
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
        void Set(std::string str);
        void Set(RuleFuncWrapper func);

        inline void Set(const char* cstr) { Set(std::string(cstr)); }

        template <class T>
        void Set(T val)
        {
            Set(RuleFuncWrapper(val));
        }

        // Getters
        inline Mode GetMode() const
        {
            if (!isAssigned)
                throw RuleTokenException("Token is uninitialized.");
            return mode;
        }

        char GetChar() const;
        std::string GetString() const;
        RuleFuncWrapper GetFunc() const;

    private:
        void DoDirectAssign(const RuleToken& other);
        void DoDirectAssign(RuleToken&& other);
        void DoNewAssign(const RuleToken& other);
        void DoNewAssign(RuleToken&& other);
        void DestroyAll();
    };
}
