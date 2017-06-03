#include "AST/Statement.h"

#include "AST/Common.h"
#include "AST/ExpressionStatement.h"
#include "AST/ReturnStatement.h"
#include "AST/VariableDeclare.h"
#include "AST/AssignmentStatement.h"
#include "AST/IfStatement.h"
#include "AST/WhileStatement.h"
#include "AST/ForStatement.h"

RULE(Statement)
{
    Autoname(builder);
    builder.Add(ReturnStatement);
    builder.Add(IfStatement);
    builder.Add(WhileStatement);
    builder.Add(ForStatement);
    builder.Add(VariableDeclaration);
    builder.Add(AssignmentStatement);
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

RULE(StatementBlockBraced)
{
    Autoname(builder);
    builder.Add('{', OptionalWhitespace, StatementBlock, OptionalWhitespace, '}');
    builder.Add(Statement);

    builder.Ignore(1);
    builder.Ignore(3);

    builder.SetNodeType<StatementBlockBracedNode>();
}


