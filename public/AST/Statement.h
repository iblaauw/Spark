#pragma once

#include "AST/RuleBase.h"

RULE(Statement);
RULE(StatementBlock);

class StatementBlockChain : public ChainingNode
{
public:
    StatementBlockChain(std::vector<NodePtr>& nodes) : ChainingNode(nodes) {}
    std::string GetType() const override { return "StatementBlockChain"; }
};

class StatementNode : public CustomNode
{
public:
    StatementNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "StatementNode"; }

    void Generate(CompileContext& context) override;
};

