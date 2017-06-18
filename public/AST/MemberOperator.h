#pragma once

#include "AST/Operator.h"

RULE(MemberOperator);

class MemberOperatorNode : public UnaryPostOperatorBase
{
public:
    MemberOperatorNode(std::vector<NodePtr>& nodes) : UnaryPostOperatorBase(nodes) {}
    std::string GetType() const override { return "MemberOperatorNode"; }

    UnknownPtr<RValue> Create(UnknownPtr<RValue> lhs, CompileContext& context) override;
};
