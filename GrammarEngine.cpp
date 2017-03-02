#include "GrammarEngine.h"

#include <sstream>

//#include "RuleBuilder.h"
#include "RuleTraverser.h"

Spark::GrammarEngine::GrammarEngine(std::istream& input) : buffer(input)
{

}

void Spark::GrammarEngine::Start(RuleFunc func)
{
    /*
    RuleBuilder builder(buffer, path);

    path.Enter();
    func(builder);
    path.Exit();
    */

    RuleToken token;
    token.Set(func);

    RuleTraverser traverser(buffer);
    traverser.Execute(token);

    if (!buffer.IsDone())
    {
        std::stringstream ss;
        ss << "At line " << buffer.LineNum() << ", character " << buffer.CharNum();
        ss << std::endl;
        ss << "Parsing finished with extra data left in the file.";
        throw ParseException(ss.str());
    }
    else
    {
        std::cout << "Success!" << std::endl;
    }
}

