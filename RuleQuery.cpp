#include "RuleQuery.h"

#include "InfoGatherer.h"
#include "Exceptions.h"

namespace Spark
{
    RuleQuery::RuleQuery(InputBuffer& input) : buffer(input), failure(false)
    {
    }

    RuleQuery::RuleQuery(SearchBuffer& current) : buffer(current), failure(false)
    {
    }

    void RuleQuery::Search(RuleToken token)
    {
        if (failure)
            return;

        switch (token.GetMode())
        {
            case RuleToken::CHAR_MODE:
                HandleChar(token.GetChar());
                break;
            case RuleToken::STRING_MODE:
                HandleString(token.GetString());
                break;
            case RuleToken::FUNC_MODE:
                HandleFunc(token.GetFunc());
                break;
            case RuleToken::CHARSET_MODE:
                HandleCharset(token.GetCharset());
                break;
            default:
                throw SparkAssertionException("UNREACHABLE");
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

    void RuleQuery::HandleString(std::string str)
    {
        for (char c : str)
        {
            HandleChar(c);

            if (failure)
                return;
        }
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

    void RuleQuery::HandleCharset(CharsetPredicate charset)
    {
        if (buffer.IsDone())
        {
            failure = true;
            return;
        }

        char current = buffer.Current();
        if (!charset(current))
        {
            failure = true;
            return;
        }

        buffer.Advance();
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
            {
                buffer.AdvanceTo(query.buffer); // Advance our buffer to same position as sub query
                return;
            }
        }

        // Every option failed
        failure = true;
    }
}
