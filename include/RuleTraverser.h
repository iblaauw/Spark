#pragma once

#include "InputBuffer.h"
#include "RuleToken.h"
#include "InfoGatherer.h"
#include "Node.h"

namespace Spark
{
    class RuleTraverser
    {
    private:
        InputBuffer& input;
        FailInfo failInfo;
    public:
        RuleTraverser(InputBuffer& input);

        NodePtr Execute(RuleToken token);
        const FailInfo& GetFailInfo() const { return failInfo; }

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
