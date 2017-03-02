#include "RuleTraverser.h"

#include <sstream>

#include "RuleQuery.h"

namespace Spark
{
    RuleTraverser::RuleTraverser(InputBuffer& input) : input(input)
    {}

    void RuleTraverser::Execute(RuleToken token)
    {
        if (token.IsFunction())
        {
            ExecuteFunc(token.GetFunc());
        }
        else
        {
            HandleChar(token.GetChar());
        }
    }

    void RuleTraverser::ExecuteFunc(RuleFuncWrapper func)
    {
        // Gather data
        InfoGatherer gatherer;
        func(gatherer);
        gatherer.Verify();

        if (gatherer.IsOption())
        {
            HandleOptions(gatherer);
            return;
        }

        if (gatherer.NumOptions() != 1)
            throw SparkAssertionException("Multiple options for non-option rule.");

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
