#pragma once

#include "AST/RuleBase.h"

RULE(ParensExpression);

class ParensExpressionNode : public CustomNode
{
public:
    ParensExpressionNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "ParensExpressionNode"; }

    UnknownPtr<RValue> Evaluate(CompileContext& context) override;
};


