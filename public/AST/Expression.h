#pragma once

#include "AST/RuleBase.h"

RULE(Expression);

class ExpressionNode : public CustomNode
{
public:
    ExpressionNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "ExpressionNode"; }

    llvm::Value* Evaluate(CompileContext& context) override;
};




