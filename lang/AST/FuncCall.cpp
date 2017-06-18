#include "AST/FuncCall.h"

#include <iostream>

#include "AST/Common.h"
#include "AST/Expression.h"


class FuncCallArgsChain : public ChainingNode
{
public:
    FuncCallArgsChain(std::vector<NodePtr>& nodes) : ChainingNode(nodes) {}
    std::string GetType() const override { return "FuncCallArgsChain"; }
};

RULE(FuncCall)
{
    Autoname(builder);
    builder.Add('(', OptionalWhitespace, FuncCallArgsList, OptionalWhitespace, ')');
    builder.Ignore(1);
    builder.Ignore(3);

    builder.SetNodeType<FuncCallNode>();
}

RULE(_FuncCallArgsChain)
{
    Autoname(builder);
    builder.Add(ExpressionTree, OptionalWhitespace, ',', OptionalWhitespace, _FuncCallArgsChain);
    builder.Add(ExpressionTree);

    builder.SetNodeType<FuncCallArgsChain>();
}

RULE(FuncCallArgsList)
{
    Autoname(builder);
    builder.Add(_FuncCallArgsChain);
    builder.AddEmpty();

    builder.SetNodeType<FuncCallArgsListNode>();
}

//// FuncCallNode ////

UnknownPtr<RValue> FuncCallNode::Create(UnknownPtr<RValue> lhs, CompileContext& context)
{
    LangType* ltype = lhs->GetType();

    SpecialOperatorImpl* opImpl = ltype->members.callOperator;
    if (opImpl == nullptr)
    {
        Error("Value of type '", ltype->GetName(), "' is not callable.");
        return nullptr;
    }

    auto argsNode = SafeGet<FuncCallArgsListNode>(0, "FuncCallArgsListNode");
    std::vector<UnknownPtr<RValue>> argValues;
    argsNode->EvalAll(context, argValues);

    return opImpl->Create(lhs, argValues, context);
}

//// FuncCallArgsListNode ////

void FuncCallArgsListNode::EvalAll(CompileContext& context, std::vector<UnknownPtr<RValue>>& evalOut)
{
    evalOut.clear();
    for (auto child : customChildren)
    {
        auto val = child->Evaluate(context);
        if (val != nullptr)
        {
            evalOut.push_back(val);
        }
    }
}



