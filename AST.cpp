#include "AST.h"
#include "LLVMManager.h"
#include "TypeConverter.h"

#include <iostream>

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


//abcdefghijklmnopqrstuvwxyz0123456789
//áâãäåæçèéêëìíîïðñòóôõö÷øùú°±²³´µ¶·¸¹

void FunctionNode::Process()
{
    ConvertToOnlyCustom();

    CustomNode::Process();
}

void FunctionNode::GatherSymbols(CompileContext& context)
{
    // TODO: make this more safe and robust, check that function doesn't already exist

    funcName = PtrCast<IdentifierNode>(customChildren[0])->GetValue();

    auto& manager = Spark::LLVMManager::Instance();
    llvm::Type* voidType = Spark::TypeConverter::Get<void>();
    auto signature = manager.GetFuncSignature(voidType);
    funcDefinition = manager.DeclareFunction(funcName, signature);

    context.symbolTable.AddFunction(funcName, funcDefinition);

    // Recurse
    CustomNode::GatherSymbols(context);
}

void FunctionNode::Generate(CompileContext& context)
{
    // TODO: make this more safe and robust, check that function does already exist

    auto& manager = Spark::LLVMManager::Instance();

    auto* bb = manager.Implement(funcDefinition);

    context.builder.SetInsertPoint(bb);

    CustomNode::Generate(context);

    context.builder.CreateRetVoid();

}

void ProgramNode::Process()
{
    ConvertToOnlyCustom();

    // Recurse
    CustomNode::Process();
}

void StatementNode::Generate(CompileContext& context)
{
    for (Ptr<CustomNode> child : customChildren)
    {
        child->Evaluate(context);
    }
}

llvm::Value* ExpressionNode::Evaluate(CompileContext& context)
{
    if (customChildren.size() == 0)
        return nullptr;

    auto child = customChildren[0];
    return child->Evaluate(context);
}

void StringLiteralNode::Process()
{
    // TODO: make more robust
    NodePtr child = children[0];
    NodePtr grandchild = (*child)[0];

    if (grandchild->GetType() != "StringNode")
        return;

    literal = PtrCast<Spark::StringNode>(grandchild)->Get();

    children.clear();
    children.push_back(grandchild);
}

llvm::Value* StringLiteralNode::Evaluate(CompileContext& context)
{
    llvm::Constant* val = llvm::ConstantDataArray::getString(
        Spark::LLVMManager::Context(),
        literal);

    auto* gvar = Spark::LLVMManager::Instance().CreateGlobalConstant(".str", val);

    auto zero = Spark::TypeConverter::Create<int>(0);
    std::vector<llvm::Value*> indices { zero, zero };

    return context.builder.CreateGEP(gvar, indices, "str_literal");
}

void FuncCallNode::Process()
{
    ConvertToOnlyCustom();
    CustomNode::Process();
}

llvm::Value* FuncCallNode::Evaluate(CompileContext& context)
{
    // TODO: make robust
    auto funcNameNode = PtrCast<IdentifierNode>(customChildren[0]);
    std::string funcName = funcNameNode->GetValue();

    auto exprNode = customChildren[1];
    llvm::Value* result = exprNode->Evaluate(context);

    if (result == nullptr)
        return nullptr;

    llvm::Function* func = context.symbolTable.GetFunction(funcName);

    if (func == nullptr)
    {
        std::cerr << "Error: a function named '" << funcName << "' could not be found." << std::endl;
        return nullptr;
    }

    std::vector<llvm::Value*> args { result };
    return context.builder.CreateCall(func, args, funcName + "_call");
}


