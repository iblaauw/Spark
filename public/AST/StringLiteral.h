#pragma once

#include "AST/RuleBase.h"

RULE(StringLiteral);

class StringLiteralNode : public CustomNode
{
private:
    std::string literal;
public:
    StringLiteralNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "StringLiteralNode"; }

    void Process() override;
    UnknownPtr<RValue> Evaluate(CompileContext& context) override;

    std::string Get() const { return literal; }

private:
    static std::string Parse(std::string val);
    static char GetSpecialValue(char val);
};

