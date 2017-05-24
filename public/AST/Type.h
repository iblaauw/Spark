#pragma once

#include "AST/RuleBase.h"

RULE(Type);

class TypeNode : public CustomNode
{
private:
    UnknownPtr<LangType> type;
public:
    TypeNode(std::vector<NodePtr>& nodes) : CustomNode(nodes), type(nullptr) {}
    std::string GetType() const override { return "TypeNode"; }

    virtual void VerifyTypes(CompileContext& context) override;

    LangType* GetIRType() { return type.Get(); }
};

