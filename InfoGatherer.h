#pragma once

#include <vector>

#include "IRuleBuilder.h"
#include "RuleToken.h"

namespace Spark
{
    class InfoGatherer : public IRuleBuilder
    {
    private:
        bool hasType;
        bool typeIsOption;
        bool newRow;

        std::vector<std::vector<RuleToken>> tokens;

    public:
        InfoGatherer();

        virtual void Add(RuleFunc func) override;
        virtual void Add(char c) override;

        inline bool IsOption() { return typeIsOption; }
        inline int NumOptions() { return tokens.size(); }

        inline std::vector<RuleToken>& Get(int index)
        {
            return tokens[index];
        }

        void Verify() const;

    protected:
        virtual void AddOptionInternal(RuleFunc func) override;
        virtual void AddOptionInternal(char c) override;
        virtual void EndOptionInternal() override;

    private:
        void TypeGuard(bool isOption);
        void TryAddNewRow();

        template <class T>
        void DoAdd(T val, bool type)
        {
            TypeGuard(type);

            TryAddNewRow();

            RuleToken token;
            token.Set(val);

            tokens.back().push_back(token);
        }
    };
}


