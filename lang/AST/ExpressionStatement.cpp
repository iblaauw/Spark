#include "AST/ExpressionStatement.h"

RULE(ExpressionStatement)
{
    // TODO: fix this, its too lenient
    Autoname(builder);
    builder.Add(ExpressionTree);
    builder.Ignore(1);

    builder.SetNodeType<ExpressionStatementNode>();
}

void ExpressionStatementNode::Generate(CompileContext& context)
{
    Assert(customChildren.size() == 1, "empty or invalid expression statement");
    customChildren[0]->Evaluate(context);
}

