#pragma once

#include "AST/RuleBase.h"

// Represents a Node that introduces a new context -> has its own symbol table
class ContextingNode : public CustomNode
{
protected:
    SymbolTable table;
public:
    ContextingNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    virtual std::string GetType() const = 0;

    void GatherTypes(CompileContext& context) override;
    void VerifyTypes(CompileContext& context) override;
    void GatherSymbols(CompileContext& context) override;
    void Generate(CompileContext& context) override;
};

