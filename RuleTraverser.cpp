#include "RuleTraverser.h"

#include <sstream>

#include "RuleQuery.h"
#include "Exceptions.h"

namespace Spark
{
    RuleTraverser::RuleTraverser(InputBuffer& input) : input(input)
    {}

    NodePtr RuleTraverser::Execute(RuleToken token)
    {
        switch (token.GetMode())
        {
            case RuleToken::CHAR_MODE:
                return HandleChar(token.GetChar());
                break;
            case RuleToken::STRING_MODE:
                return HandleString(token.GetString());
                break;
            case RuleToken::FUNC_MODE:
                return ExecuteFunc(token.GetFunc());
                break;
            case RuleToken::CHARSET_MODE:
                return HandleCharset(token.GetCharset());
                break;
            default:
                throw SparkAssertionException("UNREACHABLE");
        }

        throw SparkAssertionException("UNREACHABLE");
    }

    NodePtr RuleTraverser::ExecuteFunc(RuleFuncWrapper func)
    {
        // Gather data
        InfoGatherer gatherer;
        func(gatherer);
        gatherer.Verify();

        if (gatherer.NumOptions() > 1)
        {
            return HandleOptions(gatherer);
        }

        std::vector<NodePtr> nodes;
        for (RuleToken& tok : gatherer.Get(0))
        {
            NodePtr n = Execute(tok); // recurse
            nodes.push_back(n);
        }

        return std::make_shared<Node>(nodes);
    }

    NodePtr RuleTraverser::HandleChar(char c)
    {
        DoHandleChar(c);

        auto node = std::make_shared<CharNode>(c);
        return AsNode(node);
    }

    NodePtr RuleTraverser::HandleString(std::string str)
    {
        for (char c : str)
        {
            DoHandleChar(c);
        }

        auto node = std::make_shared<StringNode>(str);
        return AsNode(node);
    }

    NodePtr RuleTraverser::HandleOptions(InfoGatherer& gatherer)
    {
        int index = FindValidOption(gatherer);
        if (index == -1)
            throw ParseException("Failed to match rule.");

        // TODO: dispose gatherer here to free up memory
        // TODO: make a replayer to speed this up (currently will traverse bottom an exponential # times)
        std::vector<NodePtr> nodes;
        for (RuleToken tok : gatherer.Get(index))
        {
            NodePtr n = Execute(tok); // recurse
            nodes.push_back(n);
        }

        return std::make_shared<Node>(nodes);
    }

    NodePtr RuleTraverser::HandleCharset(CharsetPredicate charset)
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

        auto node = std::make_shared<CharNode>(next);
        return AsNode(node);
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

    void RuleTraverser::DoHandleChar(char c)
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


}
