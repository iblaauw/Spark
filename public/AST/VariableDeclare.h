#pragma once

#include "AST/RuleBase.h"
#include "Variable.h"

RULE(VariableDeclaration);

class VariableDeclareNode : public CustomNode
{
private:
    MemoryVariable* variable;
public:
    VariableDeclareNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "VariableDeclareNode"; }

    void GatherSymbols(CompileContext& context) override;
    void Generate(CompileContext& context) override;
};


