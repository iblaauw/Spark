#pragma once

#include "AST/RuleBase.h"

RULE(Operator);
RULE(UnaryPreOperator);
RULE(UnaryPostOperator);

class OperatorNode : public StringValueNode
{
public:
    OperatorNode(std::vector<NodePtr>& nodes) : StringValueNode(nodes) {}
    std::string GetType() const override { return "OperatorNode"; }

    UnknownPtr<RValue> Create(UnknownPtr<RValue> lhs, UnknownPtr<RValue> rhs, CompileContext& context);

    int GetPrecedence() const;
    bool IsGroupRight() const;
private:
    BinaryOperatorImpl* FindImplDir(LangType* lhs, LangType* rhs);
};

class UnaryPreOperatorNode : public StringValueNode
{
public:
    UnaryPreOperatorNode(std::vector<NodePtr>& nodes) : StringValueNode(nodes) {}
    std::string GetType() const override { return "UnaryPreOperatorNode"; }

    UnknownPtr<RValue> Create(UnknownPtr<RValue> rhs, CompileContext& context);
    UnaryOperatorImpl* FindOp(LangType* type);
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


