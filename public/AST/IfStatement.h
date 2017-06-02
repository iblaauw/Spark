#pragma once

#include "AST/RuleBase.h"
#include "AST/ContextingNode.h"

RULE(IfStatement);

class IfStatementNode : public ContextingNode
{
public:
    IfStatementNode(std::vector<NodePtr>& nodes) : ContextingNode(nodes) {}
    std::string GetType() const override { return "IfStatementNode"; }

    void Generate(CompileContext& context) override;
private:
    void Subgenerate(int index, llvm::BasicBlock* genBB, llvm::BasicBlock* contBB, CompileContext& context);
};
