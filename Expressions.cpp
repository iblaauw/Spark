#include "Expressions.h"
#include "AST.h"

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
}

RULE(Number)
{
    Autoname(builder);
    builder.AddString(NumberChar);
}

RULE(Expression)
{

}

RULE(Function)
{
    Autoname(builder);
    builder.Add("func ", Identifier, "()", OptionalWhitespace, '{', OptionalWhitespace, '}');
    builder.Ignore(3);
    builder.Ignore(5);

    builder.SetNodeType<FunctionNode>();
}

RULE(ProgramPiece)
{
    Autoname(builder);
    builder.Add(Function, OptionalWhitespace, ProgramPiece);
    builder.Add(Function);

    builder.SetNodeType<ProgramPieceNode>();
}

RULE(Program)
{
    Autoname(builder);
    builder.Add(OptionalWhitespace, ProgramPiece, OptionalWhitespace);
    builder.Ignore(0);
    builder.Ignore(2);

    builder.SetNodeType<ProgramNode>();
}






