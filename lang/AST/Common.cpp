#include "AST/Common.h"

#include <iostream>

CHARSET(Alpha)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

CHARSET(OperatorChar)
{
    return c == '+' || c == '-' || c == '*' || c == '/';
}

CHARSET(WhitespaceChar)
{
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

RULE(Whitespace)
{
    Autoname(builder);
    builder.AddString(WhitespaceChar);
}

RULE(OptionalWhitespace)
{
    Autoname(builder);
    builder.AddString(WhitespaceChar);
    builder.AddEmpty();
}

RULE(Identifier)
{
    Autoname(builder);
    builder.AddString(Alpha);

    builder.SetNodeType<IdentifierNode>();
}

RULE(Type)
{
    Autoname(builder);
    builder.AddString(Alpha);

    builder.SetNodeType<TypeNode>();
}


void TypeNode::VerifyTypes(CompileContext& context)
{
    auto* symbols = context.symbolTable;
    std::string val = GetValue();
    type = symbols->types.Get(val);

    if (type == nullptr)
    {
        Error("unknown type '", val, "'");
    }
}






