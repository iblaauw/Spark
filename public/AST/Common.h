#pragma once

#include "AST/RuleBase.h"

RULE(Whitespace);
RULE(OptionalWhitespace);
RULE(Identifier);
RULE(Type);
RULE(Number);

class IdentifierNode : public StringValueNode
{
public:
    IdentifierNode(std::vector<NodePtr>& nodes) : StringValueNode(nodes) {}
    std::string GetType() const override { return "IdentifierNode"; }
};

class TypeNode : public StringValueNode
{
public:
    TypeNode(std::vector<NodePtr>& nodes) : StringValueNode(nodes) {}
    std::string GetType() const override { return "TypeNode"; }
};

