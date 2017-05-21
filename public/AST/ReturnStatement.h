#pragma once

#include "AST/RuleBase.h"

RULE(ReturnStatement);

class ReturnStatementNode : public CustomNode
{
public:
    ReturnStatementNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "ReturnStatementNode"; }

    void Process() override;

    void Generate(CompileContext& context) override;
};

