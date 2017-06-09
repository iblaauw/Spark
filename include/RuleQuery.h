#pragma once

#include "SearchBuffer.h"
#include "RuleToken.h"
#include "InfoGatherer.h"
#include "SearchStack.h"

namespace Spark
{
    class RuleQuery
    {
    private:
        SearchBuffer buffer;
        bool failure;
        DebugContext& debugContext;
        SearchStack replayStack;
    public:
        RuleQuery(InputBuffer& input, DebugContext& debugContext);

        inline bool Failed() { return failure; }

        void Search(RuleToken token);

        SearchStack& ReplayStack() { return replayStack; }
    private:
        RuleQuery(SearchBuffer& current, DebugContext& debugContext);

        void HandleChar(char c);
        void HandleString(std::string str);
        void HandleFunc(RuleFuncWrapper func);
        void HandleCharset(CharsetPredicate charset);

        void HandleOptions(InfoGatherer& info);
    };
}
