#include "RuleTraverser.h"

#include <sstream>

#include "RuleQuery.h"
#include "Exceptions.h"
#include "ScopeGuard.h"

namespace Spark
{
    RuleTraverser::RuleTraverser(InputBuffer& input, DebugContext& debugContext)
        : input(input), debugContext(debugContext)
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

        debugContext.AddTrace(gatherer.GetName());
        debugContext.SetPosition(input.GetDebug());

        ScopeGuard _guard([this]()
        {
            debugContext.PopTrace();
        });

        if (gatherer.NumOptions() > 1)
        {
            return HandleOptions(gatherer);
        }

        return DoExecuteNodes(gatherer, 0);
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
            throw ParseException(debugContext.GetErrorMessage(input.GetRawStream()));

        return DoExecuteNodes(gatherer, index);
    }

    NodePtr RuleTraverser::HandleCharset(CharsetPredicate charset)
    {
        if (input.IsDone())
            throw ParseException("Unexpected end of file");

        char next = input.GetNext();
        if (!charset(next))
        {
            debugContext.SetPosition(input.GetDebug());

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
        if (!replayStack.empty())
        {
            int index = replayStack.Pop();
            if (index < gatherer.NumOptions())
                return index;


            std::cerr << "Warning: replay optimization is invalid, recovering but may be slower" << std::endl;
            replayStack.Clear();
        }

        for (int i = 0; i < gatherer.NumOptions(); i++)
        {
            RuleQuery query(input, debugContext);
            for (RuleToken tok : gatherer.Get(i))
            {
                query.Search(tok);
            }

            if (!query.Failed())
            {
                replayStack.Merge(query.ReplayStack());
                return i;
            }
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
            debugContext.SetPosition(input.GetDebug());

            std::stringstream ss;
            ss << "At line " << input.LineNum() << ", character " << input.CharNum() << ":";
            ss << std::endl;
            ss << "Unexpected char '" << next << "', expecting '" << c << "'.";
            throw ParseException(ss.str());
        }
    }

    // Util func
    void FlattenString(NodePtr root, std::stringstream& stream);

    NodePtr RuleTraverser::DoExecuteNodes(InfoGatherer& gatherer, int index)
    {
        // TODO: dispose gatherer here to free up memory

        int count = 0;
        const std::set<int>& ignores = gatherer.IgnoreValues();

        // Snag position before consuming children
        DebugInfo currentPosition = input.GetDebug();

        std::vector<NodePtr> nodes;
        for (RuleToken tok : gatherer.Get(index))
        {
            NodePtr n = Execute(tok); // recurse

            // If not found in ignore set
            if (ignores.count(count) == 0)
            {
                nodes.push_back(n);
            }

            count++;
        }


        if (gatherer.ShouldFlatten(index))
        {
            std::vector<NodePtr> original = nodes;
            nodes.clear();

            for (NodePtr ptr : original)
            {
                std::stringstream stream;
                FlattenString(ptr, stream);

                auto node = std::make_shared<StringNode>(stream.str());
                nodes.push_back(AsNode(node));
            }
        }

        std::string name = gatherer.GetName();

        if (gatherer.HasCustomType())
        {
            NodePtr node = gatherer.Factory(nodes);
            node->SetName(name);
            node->SetDebugInfo(currentPosition);
            return node;
        }

        return std::make_shared<Node>(nodes, name, currentPosition);
    }

    // Util func
    void FlattenString(NodePtr root, std::stringstream& stream)
    {
        std::string type = root->GetType();
        if (type == "CharNode")
        {
            auto ptr = std::static_pointer_cast<CharNode>(root);
            stream << ptr->Get();
            return;
        }

        if (type == "StringNode")
        {
            auto ptr = std::static_pointer_cast<StringNode>(root);
            stream << ptr->Get();
            return;
        }

        for (int i = 0; i < root->size(); i++)
        {
            FlattenString((*root)[i], stream);
        }
    }


}
