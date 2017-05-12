#pragma once

#include <vector>
#include <functional>

#include "llvm/IR/IRBuilder.h"

#include "Node.h"

using Spark::Node;
using Spark::NodePtr;

template <class T>
using Ptr = std::shared_ptr<T>;

template <class T, class U>
inline Ptr<T> PtrCast(Ptr<U> val)
{
    return std::static_pointer_cast<T>(val);
}


class CustomNode : public Spark::Node
{
protected:
    std::vector<Ptr<CustomNode>> customChildren;
public:
    CustomNode(std::vector<NodePtr>& nodes) : Node(nodes) { Collect(); }
    virtual bool IsBuiltin() const override { return false; }

    virtual void Process()
    {
        Map([](Ptr<CustomNode> c) { c->Process(); });
    }

    virtual void Generate()
    {
        Map([](Ptr<CustomNode> c) { c->Generate(); });
    }

private:
    void Collect()
    {
        for (NodePtr child : children)
        {
            if (!child->IsBuiltin())
            {
                customChildren.push_back(PtrCast<CustomNode>(child));
            }
        }
    }
protected:
    void ConvertToOnlyCustom()
    {
        children.clear();
        for (auto cc : customChildren)
        {
            children.push_back(AsNode(cc));
        }
    }

    void Map(std::function<void(Ptr<CustomNode>)> func)
    {
        for (auto child : customChildren)
        {
            func(child);
        }
    }
};

class FunctionNode : public CustomNode
{
public:
    std::string funcName;
public:
    FunctionNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "FunctionNode"; }
    void Process() override;
    void Generate() override;
};

class ProgramNode : public CustomNode
{
public:
    ProgramNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "ProgramNode"; }
    void Process() override;
};

class ProgramPieceNode : public CustomNode
{
public:
    ProgramPieceNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "ProgramPieceNode"; }

    void Collapse(std::vector<Ptr<CustomNode>>& funcs);
};

