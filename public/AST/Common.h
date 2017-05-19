#pragma once

#include "AST/RuleBase.h"

RULE(Whitespace);
RULE(OptionalWhitespace);
RULE(Identifier);
RULE(Type);

class IdentifierNode : public StringValueNode
{
public:
    IdentifierNode(std::vector<NodePtr>& nodes) : StringValueNode(nodes) {}
    std::string GetType() const override { return "IdentifierNode"; }
};

class TypeNode : public StringValueNode
{
private:
    LangType* type;
public:
    TypeNode(std::vector<NodePtr>& nodes) : StringValueNode(nodes) {}
    std::string GetType() const override { return "TypeNode"; }

    virtual void VerifyTypes(CompileContext& context) override;

    LangType* GetIRType() const { return type; }
};

