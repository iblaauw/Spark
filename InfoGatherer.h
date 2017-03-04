#pragma once

#include <vector>

#include "IRuleBuilder.h"
#include "RuleToken.h"

namespace Spark
{
    class InfoGatherer : public IRuleBuilder
    {
    private:
        bool newRow;
        std::vector<std::vector<RuleToken>> tokens;

    public:
        InfoGatherer();

        inline int NumOptions() { return tokens.size(); }

        inline std::vector<RuleToken>& Get(int index)
        {
            return tokens[index];
        }

        void Verify() const;

    protected:
        virtual void AddInternal(RuleToken tok) override;
        virtual void EndInternal() override;
        virtual void AddEmptyInternal() override;

    private:
        void TryAddNewRow();
    };
}


