#include "AST/Expression.h"
#include "AST/Common.h"
#include "AST/StringLiteral.h"
#include "AST/NumberLiteral.h"
#include "AST/FuncCall.h"
#include "AST/VariableExpression.h"
#include "AST/Operator.h"


RULE(Expression)
{
    Autoname(builder);
    builder.Add(NumberLiteral);
    builder.Add(StringLiteral);
    builder.Add(FuncCall);
    builder.Add(VariableExpression);

    builder.SetNodeType<ExpressionNode>();
}

RULE(ExpressionTree)
{
    Autoname(builder);
    builder.Add(Expression, OptionalWhitespace, Operator, OptionalWhitespace, ExpressionTree);
    builder.Add(UnaryPreOperator, OptionalWhitespace, ExpressionTree);
    builder.Add(Expression);

    builder.Ignore(1);
    builder.Ignore(3);

    builder.SetNodeType<ExpressionTreeNode>();
}

UnknownPtr<RValue> ExpressionNode::Evaluate(CompileContext& context)
{
    Assert(customChildren.size() != 0, "empty expression");

    auto child = customChildren[0];
    return child->Evaluate(context);
}

UnknownPtr<RValue> ExpressionTreeNode::Evaluate(CompileContext& context)
{

    if (customChildren.size() == 3)
    {
        return EvalBinary(context);
    }
    else if (customChildren.size() == 2)
    {
        return EvalUnaryPre(context);
    }
    else if (customChildren.size() == 1)
    {
        return EvalSimple(context);
    }

    Assert(customChildren.size() != 0, "empty expression");
    return nullptr;
}

UnknownPtr<RValue> ExpressionTreeNode::EvalSimple(CompileContext& context)
{
    Assert(customChildren.size() == 1, "invalid expression leaf");
    auto expr = customChildren[0]->Evaluate(context);
    return expr;
}

UnknownPtr<RValue> ExpressionTreeNode::EvalBinary(CompileContext& context)
{
    Assert(customChildren.size() == 3, "invalid binary operator expression");

    Ptr<OperatorNode> op = SafeGet<OperatorNode>(1, "OperatorNode");

    auto lhs = customChildren[0]->Evaluate(context);
    if (lhs == nullptr)
        return nullptr;

    auto rhs = customChildren[2]->Evaluate(context);
    if (rhs == nullptr)
        return nullptr;

    LangType* ltype = lhs->GetType();
    LangType* rtype = rhs->GetType();
    LangType* resultType = op->GetResultType(ltype, rtype, context);
    if (resultType == nullptr)
    {
        Error("cannot apply operator '", op->GetValue(), "' to values of type '",
                ltype->GetName(), "' and '", rtype->GetName(), "'.");
        return nullptr;
    }

    llvm::Value* lval = lhs->GetValue(context);
    llvm::Value* rval = rhs->GetValue(context);
    llvm::Value* resultValue = op->Create(lval, rval, context);
    auto finalResult = std::make_shared<GeneralRValue>(resultValue, resultType);
    return PtrCast<RValue>(finalResult);
}

UnknownPtr<RValue> ExpressionTreeNode::EvalUnaryPre(CompileContext& context)
{
    Assert(customChildren.size() == 2, "invalid unary prefix operator expression");

    Ptr<UnaryPreOperatorNode> op = SafeGet<UnaryPreOperatorNode>(0, "UnaryPreOperatorNode");

    auto rhs = customChildren[1]->Evaluate(context);
    if (rhs == nullptr)
        return nullptr;

    return op->Create(rhs, context);
}

