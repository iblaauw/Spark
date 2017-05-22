#include "AST/Statement.h"

#include "AST/Common.h"
#include "AST/ExpressionStatement.h"
#include "AST/ReturnStatement.h"
#include "AST/VariableDeclare.h"

RULE(Statement)
{
    Autoname(builder);
    builder.Add(ReturnStatement);
    builder.Add(VariableDeclaration);
    builder.Add(ExpressionStatement);

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


