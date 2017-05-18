#include "AST/Expression.h"
#include "AST/Common.h"
#include "AST/StringLiteral.h"
#include "AST/NumberLiteral.h"
#include "AST/FuncCall.h"


RULE(Expression)
{
    Autoname(builder);
    builder.Add(NumberLiteral);
    builder.Add(StringLiteral);
    builder.Add(FuncCall);

    builder.SetNodeType<ExpressionNode>();
}

Ptr<RValue> ExpressionNode::Evaluate(CompileContext& context)
{
    if (customChildren.size() == 0)
    {
        std::cerr << "Internal Error: empty expression" << std::endl;
        return nullptr;
    }

    auto child = customChildren[0];
    return child->Evaluate(context);
}


