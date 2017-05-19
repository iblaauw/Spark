#pragma once

#include "AST/RuleBase.h"

RULE(VariableExpression);

class VariableExpressionNode : public CustomNode
{
public:
    VariableExpressionNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}

    UnknownPtr<RValue> Evaluate(CompileContext& context) override;
};

