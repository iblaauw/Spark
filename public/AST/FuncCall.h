#pragma once

#include "AST/RuleBase.h"
#include "AST/Operator.h"

RULE(FuncCall);
RULE(FuncCallArgsList);

class FuncCallNode : public UnaryPostOperatorBase
{
public:
    FuncCallNode(std::vector<NodePtr>& nodes) : UnaryPostOperatorBase(nodes) {}
    std::string GetType() const override { return "FuncCallNode"; }

    UnknownPtr<RValue> Create(UnknownPtr<RValue> lhs, CompileContext& context) override;
};

class FuncCallArgsListNode : public AbsorbingNode
{
public:
    FuncCallArgsListNode(std::vector<NodePtr>& nodes) : AbsorbingNode(nodes) {}
    std::string GetType() const override { return "FuncCallArgsListNode"; }

    void EvalAll(CompileContext& context, std::vector<UnknownPtr<RValue>>& evalOut);
};


