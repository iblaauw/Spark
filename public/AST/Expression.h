#pragma once

#include "AST/RuleBase.h"

RULE(Expression);

class ExpressionNode : public CustomNode
{
public:
    ExpressionNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "ExpressionNode"; }

    Ptr<RValue> Evaluate(CompileContext& context) override;
};




