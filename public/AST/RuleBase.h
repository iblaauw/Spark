#pragma once

#include <vector>
#include <functional>

#include "IRuleBuilder.h"
#include "Node.h"
#include "SymbolTable.h"
#include "CompileContext.h"

#define RULE(name) \
void name(Spark::IRuleBuilder& builder)

#define CHARSET(name) \
bool name(char c)


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

    virtual void Process();

    virtual void GatherSymbols(CompileContext& context);

    virtual void Generate(CompileContext& context);

    virtual llvm::Value* Evaluate(CompileContext& context);

private:
    void Collect();
protected:
    void ConvertToOnlyCustom();

    void Map(std::function<void(Ptr<CustomNode>)> func);
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


