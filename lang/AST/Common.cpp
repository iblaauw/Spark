#include "AST/Common.h"

#include <iostream>

#include "AST/Comment.h"

CHARSET(WhitespaceChar)
{
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

RULE(NormalWhitespace)
{
    Autoname(builder);
    builder.AddString(WhitespaceChar);
}

RULE(Whitespace)
{
    Autoname(builder);
    builder.Add(Comment, Whitespace);
    builder.Add(NormalWhitespace, Comment, Whitespace);
    builder.Add(Comment);
    builder.Add(NormalWhitespace);
}

RULE(OptionalWhitespace)
{
    Autoname(builder);
    builder.Add(Whitespace);
    builder.AddEmpty();
}


CHARSET(IdChar)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')
            || (c == '_');
}

CHARSET(IdStartChar)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
            || (c == '_');
}

RULE(_IdEnd)
{
    Autoname(builder);
    builder.AddString(IdChar);
}

RULE(Identifier)
{
    Autoname(builder);
    builder.Add(IdStartChar, _IdEnd);
    builder.Add(IdStartChar);

    builder.SetNodeType<IdentifierNode>();
}

void IdentifierNode::Process()
{
    Assert(children.size() > 0, "Broken Identifier Node - no children");
    NodePtr startNode = children[0];
    Assert(startNode->GetType() == "CharNode", "Broken Identifier Node - invalid start node");

    char c = PtrCast<Spark::CharNode>(startNode)->Get();

    value = "";
    value += c;

    if (children.size() > 1)
    {
        NodePtr endNode = children[1];
        Assert(endNode->size() == 1, "Broken Identifier Node - empty end node");
        NodePtr subEndNode = (*endNode)[0];
        Assert(subEndNode->GetType() == "StringNode", "Broken Identifier Node - invalid sub end node");

        std::string end = PtrCast<Spark::StringNode>(subEndNode)->Get();
        value += end;
    }

    children.clear();

    auto newChild = std::make_shared<Spark::StringNode>(value);
    children.push_back(PtrCast<Spark::Node>(newChild));
}


