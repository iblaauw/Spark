#include <iostream>
#include <fstream>

#include "GrammarEngine.h"
#include "IRuleBuilder.h"

class OperatorNode : public Spark::Node
{
public:
    OperatorNode(std::vector<Spark::NodePtr>& nodes) : Node(nodes) {}
    std::string GetType() override { return "OperatorNode"; }
};


void PrintLevel(int level)
{
    for (int i = 0; i < level * 2; i++)
    {
        std::cout << ' ';
    }
}

void PrintTree(Spark::NodePtr root, int level = 0)
{
    PrintLevel(level);

    std::string type = root->GetType();
    if (type == "CharNode")
    {
        auto nodeptr = std::static_pointer_cast<Spark::CharNode>(root);
        std::cout << '\'' << nodeptr->Get() << '\'' << std::endl;
        return;
    }
    else if (type == "StringNode")
    {
        auto nodeptr = std::static_pointer_cast<Spark::StringNode>(root);
        std::cout << '"' << nodeptr->Get() << '"' << std::endl;
        return;
    }
    else
    {
        std::string name = root->GetName();
        if (name.size() == 0)
            name = "<unknown>";
        std::cout << name << " (" << type << ")" << std::endl;
    }

    Spark::Node& node = *root;

    for (int i = 0; i < root->size(); i++)
    {
        PrintTree( node[i], level+1);
    }
}



bool Alpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z');
}

void Value(Spark::IRuleBuilder& builder)
{
    Autoname(builder);
    builder.AddString(Alpha);
    builder.Add('0', '0', '9');
}

void Operator(Spark::IRuleBuilder& builder)
{
    Autoname(builder);
    builder.Add('+');
    builder.Add('-');
    builder.Add('*');
    builder.Add('/');
    builder.SetNodeType<OperatorNode>();
}

void Expression(Spark::IRuleBuilder& builder);

void ExpressionEnd(Spark::IRuleBuilder& builder)
{
    Autoname(builder);
    builder.Add(' ', Operator, ' ', Expression);
    builder.Ignore(0);
    builder.Ignore(2);
}

void Expression(Spark::IRuleBuilder& builder)
{
    Autoname(builder);
    builder.Add(Value, ExpressionEnd);
    builder.Add(Value);
}

void Program(Spark::IRuleBuilder& builder)
{
    Autoname(builder);
    builder.Add(Expression, '\n');
    builder.Ignore(1);
}

int main()
{
    std::ifstream input("input.txt");

    Spark::GrammarEngine engine(input);
    auto tree = engine.Start(Program);

    PrintTree(tree);

    return 0;
}




