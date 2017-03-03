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

        inline bool IsOption() { return typeIsOption; }
        inline int NumOptions() { return tokens.size(); }

        inline std::vector<RuleToken>& Get(int index)
        {
            return tokens[index];
        }

        void Verify() const;

    protected:
        virtual void AddInternal(RuleToken tok) override;
        virtual void AddOptionInternal(RuleToken tok) override;
        virtual void EndOptionInternal() override;

    private:
        void TypeGuard(bool isOption);
        void TryAddNewRow();
    };
}


