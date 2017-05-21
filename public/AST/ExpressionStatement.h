#pragma once

#include "AST/RuleBase.h"
#include "AST/Expression.h"

RULE(ExpressionStatement);

class ExpressionStatementNode : public CustomNode
{
public:
    ExpressionStatementNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "ExpressionStatementNode"; }

    void Generate(CompileContext& context) override;
};

