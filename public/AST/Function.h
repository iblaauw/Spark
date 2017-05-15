#pragma once

#include "AST/RuleBase.h"

RULE(Function);
RULE(FunctionParameterList);
RULE(FunctionParameter);

class FunctionNode : public CustomNode
{
private:
    std::string funcName;
    llvm::Function* funcDefinition;
public:
    FunctionNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "FunctionNode"; }
    void Process() override;
    void GatherSymbols(CompileContext& context) override;
    void Generate(CompileContext& context) override;
};

class FuncParamListNode : public CustomNode
{
public:
    FuncParamListNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "FuncParamListNode"; }
};

class FuncParameterNode : public CustomNode
{
public:
    FuncParameterNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "FuncParameterNode"; }
};