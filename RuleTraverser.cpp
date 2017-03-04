#include "RuleTraverser.h"

#include <sstream>

#include "RuleQuery.h"
#include "Exceptions.h"

namespace Spark
{
    RuleTraverser::RuleTraverser(InputBuffer& input) : input(input)
    {}

    void RuleTraverser::Execute(RuleToken token)
    {
        switch (token.GetMode())
        {
            case RuleToken::CHAR_MODE:
                HandleChar(token.GetChar());
                break;
            case RuleToken::STRING_MODE:
                HandleString(token.GetString());
                break;
            case RuleToken::FUNC_MODE:
                ExecuteFunc(token.GetFunc());
                break;
            case RuleToken::CHARSET_MODE:
                HandleCharset(token.GetCharset());
                break;
            default:
                throw SparkAssertionException("UNREACHABLE");
        }
    }

    void RuleTraverser::ExecuteFunc(RuleFuncWrapper func)
    {
        // Gather data
        InfoGatherer gatherer;
        func(gatherer);
        gatherer.Verify();

        if (gatherer.NumOptions() > 1)
        {
            HandleOptions(gatherer);
            return;
        }

        for (RuleToken& tok : gatherer.Get(0))
        {
            Execute(tok); // recurse
        }
    }

    void RuleTraverser::HandleChar(char c)
    {
        if (input.IsDone())
            throw ParseException("Unexpected end of file");

        char next = input.GetNext();
        if (next != c)
        {
            std::stringstream ss;
            ss << "At line " << input.LineNum() << ", character " << input.CharNum() << ":";
            ss << std::endl;
            ss << "Unexpected char '" << next << "', expecting '" << c << "'.";
            throw ParseException(ss.str());
        }
    }

    void RuleTraverser::HandleString(std::string str)
    {
        for (char c : str)
        {
            HandleChar(c);
        }
    }

    void RuleTraverser::HandleOptions(InfoGatherer& gatherer)
    {
        int index = FindValidOption(gatherer);
        if (index == -1)
            throw ParseException("Failed to match rule.");

        // TODO: dispose gatherer here to free up memory
        // TODO: make a replayer to speed this up (currently will traverse bottom an exponential # times)
        for (RuleToken tok : gatherer.Get(index))
        {
            Execute(tok); // recurse
        }
    }

    void RuleTraverser::HandleCharset(CharsetPredicate charset)
    {
        if (input.IsDone())
            throw ParseException("Unexpected end of file");

        char next = input.GetNext();
        if (!charset(next))
        {
            std::stringstream ss;
            ss << "At line " << input.LineNum() << ", character " << input.CharNum() << ":";
            ss << std::endl;
            ss << "Unexpected char '" << next << "'.";
            throw ParseException(ss.str());
        }
    }

    int RuleTraverser::FindValidOption(InfoGatherer& gatherer)
    {
        for (int i = 0; i < gatherer.NumOptions(); i++)
        {
            RuleQuery query(input);
            for (RuleToken tok : gatherer.Get(i))
            {
                query.Search(tok);
            }

            if (!query.Failed())
                return i;
        }

        return -1;
    }
}
