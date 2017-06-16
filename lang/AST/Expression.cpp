#include "AST/Expression.h"
#include "AST/Common.h"
#include "AST/StringLiteral.h"
#include "AST/NumberLiteral.h"
#include "AST/FuncCall.h"
#include "AST/VariableExpression.h"
#include "AST/Operator.h"
#include "AST/ParensExpression.h"
#include "AST/Indexing.h"


class ExpressionPostOpChainNode : public ChainingNode
{
public:
    ExpressionPostOpChainNode(std::vector<NodePtr>& nodes) : ChainingNode(nodes) {}
    std::string GetType() const override { return "ExpressionPostOpChainNode"; }

    UnknownPtr<RValue> EvaluateUnaryPost(UnknownPtr<RValue> expr, CompileContext& context)
    {
        UnknownPtr<RValue> result = expr;
        for (auto child : customChildren)
        {
            if (result == nullptr)
                return nullptr;

            auto op = PtrCast<UnaryPostOperatorNode>(child);
            result = op->Create(result, context);
        }
        return result;
    }
};

class ExpressionPartNode : public CustomNode
{
public:
    ExpressionPartNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "ExpressionPartNode"; }

    UnknownPtr<RValue> Evaluate(CompileContext& context) override
    {
        Assert(customChildren.size() > 1, "Invalid Node Structure");

        auto lhs = customChildren[0]->Evaluate(context);
        auto opset = SafeGet<ExpressionPostOpChainNode>(1, "ExpressionPostOpChainNode");
        return opset->EvaluateUnaryPost(lhs, context);
    }
};

RULE(Expression)
{
    Autoname(builder);
    builder.Add(NumberLiteral);
    builder.Add(StringLiteral);
    builder.Add(ParensExpression);
    builder.Add(FuncCall);
    builder.Add(IndexOf);
    builder.Add(VariableExpression);

    builder.SetNodeType<ExpressionNode>();
}

RULE(ExpressionPostOpChain)
{
    Autoname(builder);
    builder.Add(UnaryPostOperator, OptionalWhitespace, ExpressionPostOpChain);
    builder.AddEmpty();

    builder.Ignore(1);

    builder.SetNodeType<ExpressionPostOpChainNode>();
}

RULE(ExpressionPart)
{
    Autoname(builder);
    builder.Add(Expression, OptionalWhitespace, ExpressionPostOpChain);

    builder.Ignore(1);

    builder.SetNodeType<ExpressionPartNode>();
}

RULE(ExpressionTree)
{
    Autoname(builder);
    builder.Add(ExpressionPart, OptionalWhitespace, Operator, OptionalWhitespace, ExpressionTree);
    builder.Add(UnaryPreOperator, OptionalWhitespace, ExpressionTree);
    builder.Add(ExpressionPart);

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

    return op->Create(lhs, rhs, context);
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


