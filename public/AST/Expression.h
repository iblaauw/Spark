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

class ExpressionTreeNode : public CustomNode
{
public:
    ExpressionTreeNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "ExpressionTreeNode"; }

    UnknownPtr<RValue> Evaluate(CompileContext& context) override;
};



