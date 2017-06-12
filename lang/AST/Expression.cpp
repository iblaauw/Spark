#include "AST/Expression.h"
#include "AST/Common.h"
#include "AST/StringLiteral.h"
#include "AST/NumberLiteral.h"
#include "AST/FuncCall.h"
#include "AST/VariableExpression.h"
#include "AST/Operator.h"
#include "AST/ParensExpression.h"


RULE(Expression)
{
    Autoname(builder);
    builder.Add(NumberLiteral);
    builder.Add(StringLiteral);
    builder.Add(ParensExpression);
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

void ExpressionTreeNode::Process()
{
    CustomNode::Process();

    if (customChildren.size() != 3)
        return;

    auto op = SafeGet<OperatorNode>(1, "OperatorNode");
    precedence = op->GetPrecedence();

    auto rhs = SafeGet<ExpressionTreeNode>(2, "ExpressionTreeNode");
    int otherPrecedence = rhs->GetPrecedence();

    if (precedence == 100 || otherPrecedence == 100)
        return;

    if (precedence > otherPrecedence
        || (precedence == otherPrecedence && !op->IsGroupRight()))
    {
        DoRotate(op, rhs);
    }
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

void ExpressionTreeNode::DoRotate(Ptr<OperatorNode> op, Ptr<ExpressionTreeNode> child)
{
    Assert(customChildren.size() == 3, "invalid expression tree structure");

    auto lhs = customChildren[0];
    auto op2 = child->GetBinaryOperator();
    auto rhs = child->GetRHS();

    child->DoLowerRotate(lhs, op);

    customChildren[0] = PtrCast<CustomNode>(child);
    customChildren[1] = PtrCast<CustomNode>(op2);
    customChildren[2] = rhs;

    children[0] = PtrCast<Spark::Node>(customChildren[0]);
    children[1] = PtrCast<Spark::Node>(customChildren[1]);
    children[2] = PtrCast<Spark::Node>(customChildren[2]);

    precedence = op2->GetPrecedence();
}

void ExpressionTreeNode::DoLowerRotate(Ptr<CustomNode> newLHS, Ptr<OperatorNode> op)
{
    Assert(customChildren.size() == 3, "invalid expression tree structure");

    auto rhs = customChildren[0];

    customChildren[0] = newLHS;
    customChildren[1] = PtrCast<CustomNode>(op);
    customChildren[2] = rhs;

    children[0] = PtrCast<Spark::Node>(customChildren[0]);
    children[1] = PtrCast<Spark::Node>(customChildren[1]);
    children[2] = PtrCast<Spark::Node>(customChildren[2]);

    precedence = op->GetPrecedence();
}

Ptr<OperatorNode> ExpressionTreeNode::GetBinaryOperator()
{
    Assert(customChildren.size() == 3, "Invalid expression tree structure");
    return SafeGet<OperatorNode>(1, "OperatorNode");
}

Ptr<CustomNode> ExpressionTreeNode::GetRHS()
{
    Assert(customChildren.size() == 3, "Invalid expression tree structure");
    return customChildren[2];
}


