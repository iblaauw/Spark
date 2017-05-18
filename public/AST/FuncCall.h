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

    Ptr<RValue> Evaluate(CompileContext& context) override;

private:
    bool IsCompatible(const std::vector<Ptr<RValue>>& args, Ptr<Function> func);
};

class FuncCallArgsListNode : public AbsorbingNode
{
public:
    FuncCallArgsListNode(std::vector<NodePtr>& nodes) : AbsorbingNode(nodes) {}
    std::string GetType() const override { return "FuncCallArgsListNode"; }

    void EvalAll(CompileContext& context, std::vector<Ptr<RValue>>& evalOut);
};


