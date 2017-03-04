#pragma once

#include "SearchBuffer.h"
#include "RuleToken.h"
#include "InfoGatherer.h"

namespace Spark
{
    class RuleQuery
    {
    private:
        SearchBuffer buffer;
        bool failure;
    public:
        RuleQuery(InputBuffer& input);

        inline bool Failed() { return failure; }

        void Search(RuleToken token);
    private:
        RuleQuery(SearchBuffer& current);

        void HandleChar(char c);
        void HandleString(std::string str);
        void HandleFunc(RuleFuncWrapper func);
        void HandleCharset(CharsetPredicate charset);

        void HandleOptions(InfoGatherer& info);

    };
}
