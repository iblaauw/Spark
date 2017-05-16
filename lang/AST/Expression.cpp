#include "AST/Expression.h"
#include "AST/Common.h"
#include "AST/StringLiteral.h"
#include "AST/FuncCall.h"


RULE(Expression)
{
    Autoname(builder);
    builder.Add(Number);
    builder.Add(StringLiteral);
    builder.Add(FuncCall);

    builder.SetNodeType<ExpressionNode>();
}

Ptr<RValue> ExpressionNode::Evaluate(CompileContext& context)
{
    if (customChildren.size() == 0)
    {
        auto error = std::make_shared<ErrorValue>();
        return PtrCast<RValue>(error);
    }

    auto child = customChildren[0];
    return child->Evaluate(context);
}


