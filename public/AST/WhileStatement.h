#pragma once

#include "AST/RuleBase.h"
#include "AST/ContextingNode.h"

RULE(WhileStatement);

class WhileStatementNode : public ContextingNode
{
public:
    WhileStatementNode(std::vector<NodePtr>& nodes) : ContextingNode(nodes) {}
    std::string GetType() const override { return "WhileStatementNode"; }

    void Generate(CompileContext& context) override;
};
