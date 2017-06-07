#include "GrammarEngine.h"

#include <sstream>

#include "RuleTraverser.h"
#include "Exceptions.h"

Spark::GrammarEngine::GrammarEngine(std::istream& input) : buffer(input)
{

}

Spark::NodePtr Spark::GrammarEngine::Start(RuleFuncWrapper func)
{
    RuleToken token;
    token.Set(func);

    RuleTraverser traverser(buffer);
    NodePtr tree = traverser.Execute(token);

    if (!buffer.IsDone())
    {
        std::stringstream ss;
        ss << traverser.GetFailInfo().GetErrorMessage();
        ss << std::endl;
        ss << "At line " << buffer.LineNum() << ", character " << buffer.CharNum();
        ss << std::endl;
        ss << "Parsing finished with extra data left in the file.";
        throw ParseException(ss.str());
    }
    else
    {
        std::cout << "Success!" << std::endl;
        return tree;
    }
}

