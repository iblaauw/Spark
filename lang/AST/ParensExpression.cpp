#include "AST/ParensExpression.h"

#include "AST/Expression.h"
#include "AST/Common.h"

RULE(ParensExpression)
{
    Autoname(builder);
    builder.Add('(', OptionalWhitespace, ExpressionTree, OptionalWhitespace, ')');

    builder.Ignore(1);
    builder.Ignore(3);

    builder.SetNodeType<ParensExpressionNode>();
}


UnknownPtr<RValue> ParensExpressionNode::Evaluate(CompileContext& context)
{
    Assert(customChildren.size() >= 1, "Invalid parens expression structure.");
    return customChildren[0]->Evaluate(context);
}


