#pragma once

#include "AST/RuleBase.h"

RULE(IndexOf);

class IndexOfNode : public CustomNode
{
public:
    IndexOfNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "IndexOfNode"; }

    UnknownPtr<RValue> Evaluate(CompileContext& context) override;
};

