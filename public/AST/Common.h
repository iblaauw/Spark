#pragma once

#include "AST/RuleBase.h"

RULE(Whitespace);
RULE(OptionalWhitespace);
RULE(Identifier);

RULE(Type); // defined fully in AST/Type.h

class IdentifierNode : public CustomNode
{
private:
    std::string value;
public:
    IdentifierNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "IdentifierNode"; }

    void Process() override;

    std::string GetValue() const { return value; }

};

