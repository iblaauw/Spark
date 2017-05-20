#pragma once

#include "AST/RuleBase.h"

RULE(Operator);

class OperatorImpl;

class OperatorNode : public StringValueNode
{
private:
    Ptr<OperatorImpl> impl;
public:
    OperatorNode(std::vector<NodePtr>& nodes) : StringValueNode(nodes) {}
    std::string GetType() const override { return "OperatorNode"; }
    void Process() override;

    LangType* GetResultType(LangType* lhs, LangType* rhs, CompileContext& context); // return nullptr if incompatible
    llvm::Value* Create(llvm::Value* lhs, llvm::Value* rhs, CompileContext& context);
};

