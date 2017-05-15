#include "AST/Common.h"

CHARSET(Alpha)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

CHARSET(OperatorChar)
{
    return c == '+' || c == '-' || c == '*' || c == '/';
}

CHARSET(NumberChar)
{
    return c >= '0' && c <= '9';
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

RULE(Number)
{
    Autoname(builder);
    builder.AddString(NumberChar);
}


