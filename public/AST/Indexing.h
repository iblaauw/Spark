#pragma once

#include "AST/RuleBase.h"
#include "AST/Operator.h"

RULE(IndexOf);

class IndexOfNode : public UnaryPostOperatorBase
{
public:
    IndexOfNode(std::vector<NodePtr>& nodes) : UnaryPostOperatorBase(nodes) {}
    std::string GetType() const override { return "IndexOfNode"; }

    UnknownPtr<RValue> Create(UnknownPtr<RValue> lhs, CompileContext& context) override;
};

