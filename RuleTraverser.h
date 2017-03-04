#pragma once

#include "InputBuffer.h"
#include "RuleToken.h"
#include "InfoGatherer.h"

namespace Spark
{
    class RuleTraverser
    {
    private:
        InputBuffer& input;
    public:
        RuleTraverser(InputBuffer& input);

        void Execute(RuleToken token);

    private:
        void ExecuteFunc(RuleFuncWrapper func);

        void HandleChar(char c);
        void HandleString(std::string str);
        void HandleCharset(CharsetPredicate charset);
        void HandleOptions(InfoGatherer& gatherer);

        int FindValidOption(InfoGatherer& gatherer);
    };
}
