#pragma once

#include "AST/RuleBase.h"

RULE(AssignmentStatement);

class AssignmentStatementNode : public CustomNode
{
public:
    AssignmentStatementNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "AssignmentStatementNode"; }

    void Generate(CompileContext& context);
};

