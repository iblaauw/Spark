#pragma once

#include "AST/RuleBase.h"

RULE(NumberLiteral);

class NumberLiteralNode : public StringValueNode
{
private:
    int value;
public:
    NumberLiteralNode(std::vector<NodePtr>& nodes) : StringValueNode(nodes) {}
    std::string GetType() const override { return "NumberLiteralNode"; }
    void Process() override;

    UnknownPtr<RValue> Evaluate(CompileContext& context) override;

    int GetNumber() const { return value; }
};

