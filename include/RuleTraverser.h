#pragma once

#include "InputBuffer.h"
#include "RuleToken.h"
#include "InfoGatherer.h"
#include "Node.h"
#include "SearchStack.h"

namespace Spark
{
    class RuleTraverser
    {
    private:
        InputBuffer& input;
        DebugContext& debugContext;
        SearchStack replayStack;
    public:
        RuleTraverser(InputBuffer& input, DebugContext& debugContext);

        NodePtr Execute(RuleToken token);

    private:
        NodePtr ExecuteFunc(RuleFuncWrapper func);

        NodePtr HandleChar(char c);
        NodePtr HandleString(std::string str);
        NodePtr HandleCharset(CharsetPredicate charset);
        NodePtr HandleOptions(InfoGatherer& gatherer);

        int FindValidOption(InfoGatherer& gatherer);
        void DoHandleChar(char c);
        NodePtr DoExecuteNodes(InfoGatherer& gatherer, int index);
    };
}
