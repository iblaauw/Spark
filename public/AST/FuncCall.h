#pragma once

#include "AST/RuleBase.h"

RULE(FuncCall);


class FuncCallNode : public CustomNode
{
public:
    FuncCallNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "FuncCallNode"; }
    void Process() override;

    llvm::Value* Evaluate(CompileContext& context) override;
};


