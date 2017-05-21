#include "AST/ExpressionStatement.h"

RULE(ExpressionStatement)
{
    // TODO: fix this, its too lenient
    Autoname(builder);
    builder.Add(ExpressionTree, ';');
    builder.Ignore(1);

    builder.SetNodeType<ExpressionStatementNode>();
}

void ExpressionStatementNode::Generate(CompileContext& context)
{
    if (customChildren.size() != 1)
    {
        std::cerr << "Internal Error: Empty or invalid expression statement" << std::endl;
        return;
    }

    customChildren[0]->Evaluate(context);
}

