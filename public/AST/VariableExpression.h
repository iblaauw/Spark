#pragma once

#include "AST/RuleBase.h"

RULE(VariableExpression);

class VariableExpressionNode : public CustomNode
{
public:
    VariableExpressionNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "VariableExpressionNode"; }

    UnknownPtr<RValue> Evaluate(CompileContext& context) override;
};

