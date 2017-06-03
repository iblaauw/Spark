#pragma once

#include "AST/RuleBase.h"
#include "AST/ContextingNode.h"

RULE(ForStatement);

class ForStatementNode : public ContextingNode
{
public:
    ForStatementNode(std::vector<NodePtr>& nodes) : ContextingNode(nodes) {}
    std::string GetType() const override { return "ForStatementNode"; }

    void Generate(CompileContext& context) override;
};


