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

CHARSET(StringLiteralChar)
{
    return c != '"' && c != '\n';
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
}

RULE(Number)
{
    Autoname(builder);
    builder.AddString(NumberChar);
}

RULE(_StringContents)
{
    Autoname(builder);
    builder.AddString(StringLiteralChar);
}

RULE(StringLiteral)
{
    Autoname(builder);
    builder.Add('"', _StringContents, '"');
    builder.Ignore(0);
    builder.Ignore(2);

    builder.SetNodeType<StringLiteralNode>();
}

RULE(FuncCall)
{
    Autoname(builder);
    builder.Add(Identifier, '(', Expression, ')');

    builder.SetNodeType<FuncCallNode>();
}

RULE(Expression)
{
    Autoname(builder);
    builder.Add(Number);
    builder.Add(StringLiteral);
    builder.Add(FuncCall);

    builder.SetNodeType<ExpressionNode>();
}

RULE(Statement)
{
    Autoname(builder);
    // TODO: fix this, its too lenient
    builder.Add(Expression, ';');
    builder.Ignore(1);

    builder.SetNodeType<StatementNode>();
}

RULE(StatementBlock)
{
    Autoname(builder);
    builder.Add(Statement, OptionalWhitespace, StatementBlock);
    builder.AddEmpty();

    builder.Ignore(1);

    builder.SetNodeType<StatementBlockChain>();
}


RULE(ProgramPiece)
{
    Autoname(builder);
    builder.Add(Function, OptionalWhitespace, ProgramPiece);
    builder.Add(Function);

    builder.SetNodeType<ProgramPieceChain>();
}

RULE(Program)
{
    Autoname(builder);
    builder.Add(OptionalWhitespace, ProgramPiece, OptionalWhitespace);
    builder.Ignore(0);
    builder.Ignore(2);

    builder.SetNodeType<ProgramNode>();
}

// Function Declarations

RULE(Function)
{
    Autoname(builder);
    builder.Add("func ", Identifier, '(', FunctionParameterList, ')', OptionalWhitespace, '{', OptionalWhitespace, StatementBlock, OptionalWhitespace, '}');

    builder.Ignore(5);
    builder.Ignore(7);
    builder.Ignore(9);

    builder.SetNodeType<FunctionNode>();
}

RULE(FunctionParameterChain)
{
    Autoname(builder);
    builder.Add(FunctionParameter, OptionalWhitespace, ',', OptionalWhitespace, FunctionParameterChain);
    builder.Add(FunctionParameter);

    builder.Ignore(1);
    builder.Ignore(2);
    builder.Ignore(3);

    builder.SetNodeType<FuncParamChain>();
}

RULE(FunctionParameterList)
{
    Autoname(builder);
    builder.Add(OptionalWhitespace, FunctionParameterChain, OptionalWhitespace);
    builder.AddEmpty();

    builder.Ignore(0);
    builder.Ignore(2);

    builder.SetNodeType<FuncParamListNode>();
}

RULE(FunctionParameter)
{
    Autoname(builder);
    builder.Add(Type, Whitespace, Identifier);

    builder.Ignore(1);

    builder.SetNodeType<FuncParameterNode>();
}



