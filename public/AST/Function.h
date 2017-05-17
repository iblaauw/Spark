#pragma once

#include "AST/RuleBase.h"
#include "AST/Common.h"
#include "Function.h"

RULE(FunctionRule);
RULE(FunctionParameterList);
RULE(FunctionParameter);

class FunctionNode : public CustomNode
{
private:
    Function* funcDefinition;
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

    void GetParamTypes(std::vector<LangType*>& vecOut);
    void GetParamNames(std::vector<std::string>& vecOut);
};

class FuncParameterNode : public CustomNode
{
public:
    FuncParameterNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "FuncParameterNode"; }

    Ptr<IdentifierNode> GetIdentifier() const;
    Ptr<TypeNode> GetParamType() const;
};
