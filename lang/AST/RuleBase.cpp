#include "AST/RuleBase.h"

#include <iostream>

void CustomNode::Process()
{
    Map([](Ptr<CustomNode> c) { c->Process(); });
}

void CustomNode::GatherTypes(CompileContext& context)
{
    Map([&](Ptr<CustomNode> c) { c->GatherTypes(context); });
}

void CustomNode::VerifyTypes(CompileContext& context)
{
    Map([&](Ptr<CustomNode> c) { c->VerifyTypes(context); });
}

void CustomNode::GatherSymbols(CompileContext& context)
{
    Map([&](Ptr<CustomNode> c) { c->GatherSymbols(context); });
}

void CustomNode::Generate(CompileContext& context)
{
    Map([&](Ptr<CustomNode> c) { c->Generate(context); });
}

Ptr<RValue> CustomNode::Evaluate(CompileContext& context)
{
    std::cerr << "Error: node '" << name << "' cannot be evaluated." << std::endl;
    return nullptr;
}

void CustomNode::Collect()
{
    for (NodePtr child : children)
    {
        if (!child->IsBuiltin())
        {
            customChildren.push_back(PtrCast<CustomNode>(child));
        }
    }
}

void CustomNode::ConvertToOnlyCustom()
{
    children.clear();
    for (auto cc : customChildren)
    {
        children.push_back(AsNode(cc));
    }
}

void CustomNode::Map(std::function<void(Ptr<CustomNode>)> func)
{
    for (auto child : customChildren)
    {
        func(child);
    }
}

void StringValueNode::Process()
{
    value = "<error>";

    if (children.size() == 0)
        return;

    NodePtr name = children[0];
    if (name->GetType() != "StringNode")
        return;

    auto val = PtrCast<Spark::StringNode>(name);
    value = val->Get();
}

void ChainingNode::Process()
{
    std::vector<Ptr<CustomNode>> values;
    Collapse(values);

    customChildren.clear();
    customChildren.insert(customChildren.end(), values.begin(), values.end());

    ConvertToOnlyCustom();

    // Recurse
    CustomNode::Process();
}

void ChainingNode::Collapse(std::vector<Ptr<CustomNode>>& values)
{
    if (customChildren.size() == 0)
        return;

    Ptr<CustomNode> statement = customChildren[0];
    values.push_back(statement);

    if (customChildren.size() <= 1)
        return;

    Ptr<CustomNode> child = customChildren[1];

    // TODO: make this safer with a dynamic cast

    Ptr<ChainingNode> realChild = PtrCast<ChainingNode>(child);
    // Recurse
    realChild->Collapse(values);
}

