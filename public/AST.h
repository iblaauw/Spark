#pragma once

#include <vector>
#include <functional>

#include "llvm/IR/IRBuilder.h"

#include "Node.h"
#include "SymbolTable.h"
#include "CompileContext.h"

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

    virtual void GatherSymbols(CompileContext& context)
    {
        Map([&](Ptr<CustomNode> c) { c->GatherSymbols(context); });
    }

    virtual void Generate(CompileContext& context)
    {
        Map([&](Ptr<CustomNode> c) { c->Generate(context); });
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

class IdentifierNode : public StringValueNode
{
public:
    IdentifierNode(std::vector<NodePtr>& nodes) : StringValueNode(nodes) {}
    std::string GetType() const override { return "IdentifierNode"; }
};

class FunctionNode : public CustomNode
{
private:
    std::string funcName;
    llvm::Function* funcDefinition;
public:
    FunctionNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "FunctionNode"; }
    void Process() override;
    void GatherSymbols(CompileContext& context) override;
    void Generate(CompileContext& context) override;
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

class StatementBlockNode : public CustomNode
{
public:
    StatementBlockNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "StatementBlockNode"; }

    void Process() override;
    void Collapse(std::vector<Ptr<CustomNode>>& statements);
};

class StatementNode : public CustomNode
{
public:
    StatementNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "StatementNode"; }

    void Generate(CompileContext& context) override;
};

class EvaluatableNode : public CustomNode
{
public:
    EvaluatableNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const = 0;

    virtual llvm::Value* Evaluate(CompileContext& context) = 0;
};

class ExpressionNode : public EvaluatableNode
{
public:
    ExpressionNode(std::vector<NodePtr>& nodes) : EvaluatableNode(nodes) {}
    std::string GetType() const override { return "ExpressionNode"; }

    llvm::Value* Evaluate(CompileContext& context) override;
};

class StringLiteralNode : public EvaluatableNode
{
private:
    std::string literal;
public:
    StringLiteralNode(std::vector<NodePtr>& nodes) : EvaluatableNode(nodes) {}
    std::string GetType() const override { return "StringLiteralNode"; }

    void Process() override;
    llvm::Value* Evaluate(CompileContext& context) override;

    std::string Get() const { return literal; }
};

class FuncCallNode : public EvaluatableNode
{
public:
    FuncCallNode(std::vector<NodePtr>& nodes) : EvaluatableNode(nodes) {}
    std::string GetType() const override { return "FuncCallNode"; }
    void Process() override;

    llvm::Value* Evaluate(CompileContext& context) override;
};

