#include "RuleQuery.h"

#include "InfoGatherer.h"

namespace Spark
{
    RuleQuery::RuleQuery(InputBuffer& input) : buffer(input), failure(false)
    {}

    RuleQuery::RuleQuery(SearchBuffer& current) : buffer(current), failure(false)
    {}

    void RuleQuery::Search(RuleToken token)
    {
        if (failure)
            return;

        if (token.IsFunction())
        {
            HandleFunc(token.GetFunc());
        }
        else
        {
            HandleChar(token.GetChar());
        }
    }

    void RuleQuery::HandleChar(char c)
    {
        if (buffer.IsDone())
        {
            failure = true;
            return;
        }

        char current = buffer.Current();
        if (c != current)
        {
            failure = true;
            return;
        }

        buffer.Advance();
    }

    void RuleQuery::HandleFunc(RuleFuncWrapper func)
    {
        // Gather info
        InfoGatherer info;
        func(info);
        info.Verify();

        if (info.IsOption())
        {
            HandleOptions(info);
        }
        else
        {
            for (RuleToken tok : info.Get(0))
            {
                Search(tok);
            }
        }
    }

    void RuleQuery::HandleOptions(InfoGatherer& info)
    {
        // All that is needed is for at least one option to succeed and
        //  then this branch of the search succeeds
        for (int i = 0; i < info.NumOptions(); i++)
        {
            RuleQuery query(buffer);
            for (RuleToken tok : info.Get(i))
            {
                query.Search(tok);
            }

            if (!query.Failed()) // It succeeded! End the search
                return;
        }

        // Every option failed
        failure = true;
    }
}
