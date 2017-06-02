#pragma once

#include "AST/RuleBase.h"

RULE(Statement);
RULE(StatementBlock);
RULE(StatementBlockBraced);

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
};

class StatementBlockBracedNode : public CustomNode
{
public:
    StatementBlockBracedNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "StatementBlockBracedNode"; }
};

