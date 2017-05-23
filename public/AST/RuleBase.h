#pragma once

#include <vector>
#include <functional>

#include "PtrUtils.h"
#include "IRuleBuilder.h"
#include "Node.h"
#include "CompileContext.h"
#include "LangValue.h"
#include "UnknownPtr.h"
#include "Errors.h"

#include <iostream>

#define RULE(name) \
void name(Spark::IRuleBuilder& builder)

#define CHARSET(name) \
bool name(char c)


using Spark::Node;
using Spark::NodePtr;


class CustomNode : public Spark::Node
{
protected:
    std::vector<Ptr<CustomNode>> customChildren;
public:
    CustomNode(std::vector<NodePtr>& nodes) : Node(nodes) { Collect(); }
    virtual bool IsBuiltin() const override { return false; }

    virtual void Process();

    virtual void GatherTypes(CompileContext& context);

    virtual void VerifyTypes(CompileContext& context);

    virtual void GatherSymbols(CompileContext& context);

    virtual void Generate(CompileContext& context);

    virtual UnknownPtr<RValue> Evaluate(CompileContext& context);

    const std::vector<Ptr<CustomNode>>& GetCustom() const { return customChildren; }

private:
    void Collect();
protected:
    void ConvertToOnlyCustom();

    void Map(std::function<void(Ptr<CustomNode>)> func);

    template <class T>
    Ptr<T> SafeGet(int index) const
    {
        Assert(customChildren.size() > index, "Invalid Node Structure");
        return PtrCast<T>(customChildren[index]);
    }

    template <class T>
    Ptr<T> SafeGet(int index, std::string type) const
    {
        Assert(customChildren.size() > index, "Invalid Node Structure");

        Ptr<CustomNode> child = customChildren[index];

        Assert(child->GetType() == type, "Invalid Node Structure");

        return PtrCast<T>(child);
    }
};

class StringValueNode : public CustomNode
{
private:
    std::string value;
public:
    StringValueNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    virtual std::string GetType() const = 0;

    virtual void Process() override;

    std::string GetValue() { return value; }
};

class ChainingNode : public CustomNode
{
public:
    ChainingNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    virtual std::string GetType() const = 0;

    virtual void Process() override;
private:
    void Collapse(std::vector<Ptr<CustomNode>>& vals);
};

class AbsorbingNode : public CustomNode
{
public:
    AbsorbingNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    virtual std::string GetType() const = 0;

    virtual void Process() override;
};


