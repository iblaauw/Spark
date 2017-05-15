#include "AST/Statement.h"

#include "AST/Common.h"
#include "AST/Expression.h"

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


void StatementNode::Generate(CompileContext& context)
{
    for (Ptr<CustomNode> child : customChildren)
    {
        child->Evaluate(context);
    }
}


