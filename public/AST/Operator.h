#pragma once

#include "AST/RuleBase.h"

RULE(Operator);
RULE(UnaryPreOperator);
RULE(UnaryPostOperator);

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

    int GetPrecedence() const;
    bool IsGroupRight() const;
};

class UnaryOperatorImpl;

class UnaryPreOperatorNode : public StringValueNode
{
private:
    Ptr<UnaryOperatorImpl> impl;
public:
    UnaryPreOperatorNode(std::vector<NodePtr>& nodes) : StringValueNode(nodes) {}
    std::string GetType() const override { return "UnaryPreOperatorNode"; }
    void Process() override;

    UnknownPtr<RValue> Create(UnknownPtr<RValue> rhs, CompileContext& context);
};

class UnaryPostOperatorBase : public CustomNode
{
public:
    UnaryPostOperatorBase(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    virtual UnknownPtr<RValue> Create(UnknownPtr<RValue> lhs, CompileContext& context) = 0;
};

class UnaryPostOperatorNode : public CustomNode
{
public:
    UnaryPostOperatorNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "UnaryPostOperatorNode"; }

    UnknownPtr<RValue> Create(UnknownPtr<RValue> lhs, CompileContext& context);
};

