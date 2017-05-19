#pragma once

#include "AST/RuleBase.h"

RULE(FuncCall);
RULE(FuncCallArgsList);


class FuncCallNode : public CustomNode
{
public:
    FuncCallNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "FuncCallNode"; }
    void Process() override;

    UnknownPtr<RValue> Evaluate(CompileContext& context) override;

private:
    bool IsCompatible(const std::vector<UnknownPtr<RValue>>& args, Function* func);
};

class FuncCallArgsListNode : public AbsorbingNode
{
public:
    FuncCallArgsListNode(std::vector<NodePtr>& nodes) : AbsorbingNode(nodes) {}
    std::string GetType() const override { return "FuncCallArgsListNode"; }

    void EvalAll(CompileContext& context, std::vector<UnknownPtr<RValue>>& evalOut);
};


