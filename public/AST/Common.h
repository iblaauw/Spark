#pragma once

#include "AST/RuleBase.h"

CHARSET(Alpha);
RULE(Whitespace);
RULE(OptionalWhitespace);
RULE(Identifier);

RULE(Type); // defined fully in AST/Type.h

class IdentifierNode : public StringValueNode
{
public:
    IdentifierNode(std::vector<NodePtr>& nodes) : StringValueNode(nodes) {}
    std::string GetType() const override { return "IdentifierNode"; }
};

