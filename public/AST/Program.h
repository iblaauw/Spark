#pragma once

#include "AST/RuleBase.h"

RULE(ProgramPiece);
RULE(Program);


class ProgramNode : public CustomNode
{
public:
    ProgramNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "ProgramNode"; }
    void Process() override;
};

class ProgramPieceChain : public ChainingNode
{
public:
    ProgramPieceChain(std::vector<NodePtr>& nodes) : ChainingNode(nodes) {}
    std::string GetType() const override { return "ProgramPieceChain"; }
};

