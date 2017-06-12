#pragma once

#include "AST/RuleBase.h"

RULE(ExpressionTree);

class ExpressionNode : public CustomNode
{
public:
    ExpressionNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "ExpressionNode"; }

    UnknownPtr<RValue> Evaluate(CompileContext& context) override;
};

class OperatorNode;

class ExpressionTreeNode : public CustomNode
{
private:
    int precedence;
public:
    ExpressionTreeNode(std::vector<NodePtr>& nodes) : CustomNode(nodes), precedence(100) {}
    std::string GetType() const override { return "ExpressionTreeNode"; }

    void Process() override;

    UnknownPtr<RValue> Evaluate(CompileContext& context) override;

    int GetPrecedence() const { return precedence; }

private:
    UnknownPtr<RValue> EvalSimple(CompileContext& context);
    UnknownPtr<RValue> EvalBinary(CompileContext& context);
    UnknownPtr<RValue> EvalUnaryPre(CompileContext& context);
    //UnknownPtr<RValue> EvalUnaryPost(CompileContext& context);

    void DoRotate(Ptr<OperatorNode> op, Ptr<ExpressionTreeNode> child);
    void DoLowerRotate(Ptr<CustomNode> newLHS, Ptr<OperatorNode> op);

    Ptr<OperatorNode> GetBinaryOperator();
    Ptr<CustomNode> GetRHS();
};



