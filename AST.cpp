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
    NodePtr child = children[0];

    customChildren.clear();

    PtrCast<ProgramPieceNode>(child)->Collapse(customChildren);

    ConvertToOnlyCustom();

    // Recurse
    CustomNode::Process();
}

void ProgramPieceNode::Collapse(std::vector<Ptr<CustomNode>>& funcs)
{
    NodePtr func = children[0];
    if (func->GetType() == "FunctionNode")
    {
        funcs.push_back(PtrCast<CustomNode>(func));
    }

    if (children.size() < 3)
        return;

    NodePtr child = children[2];
    if (child->GetType() != "ProgramPieceNode")
        return;

    Ptr<ProgramPieceNode> realChild = PtrCast<ProgramPieceNode>(children[2]);
    realChild->Collapse(funcs);
}

void StatementBlockNode::Process()
{
    std::vector<Ptr<CustomNode>> statements;
    Collapse(statements);

    customChildren.clear();
    customChildren.insert(customChildren.end(), statements.begin(), statements.end());

    ConvertToOnlyCustom();

    // Recurse
    CustomNode::Process();
}

void StatementBlockNode::Collapse(std::vector<Ptr<CustomNode>>& statements)
{
    if (customChildren.size() == 0)
        return;

    Ptr<CustomNode> statement = customChildren[0];
    if (statement->GetType() == "StatementNode")
    {
        statements.push_back(statement);
    }

    if (customChildren.size() <= 1)
        return;

    Ptr<CustomNode> child = customChildren[1];

    if (child->GetType() != "StatementBlockNode")
        return;

    Ptr<StatementBlockNode> realChild = PtrCast<StatementBlockNode>(child);
    // Recurse
    realChild->Collapse(statements);
}

void StatementNode::Generate(CompileContext& context)
{
    for (Ptr<CustomNode> child : customChildren)
    {
        PtrCast<ExpressionNode>(child)->Evaluate(context);
    }
}

llvm::Value* ExpressionNode::Evaluate(CompileContext& context)
{
    if (customChildren.size() == 0)
        return nullptr;

    NodePtr child = customChildren[0];
    auto realChild = PtrCast<EvaluatableNode>(child);
    return realChild->Evaluate(context);
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

    llvm::GlobalVariable* gvar = new llvm::GlobalVariable(
        Spark::LLVMManager::Context(), val->getType(), true, llvm::GlobalVariable::LinkageTypes::PrivateLinkage, val, ".str");

    auto zero = Spark::TypeConverter::Create<int>(0);
    std::vector<llvm::Value*> indices { zero, zero };

    //llvm::Type* strType = val->getType();
    //llvm::Type* cstrType = strType->getArrayElementType()->getPointerTo();

    return context.builder.CreateGEP(gvar, indices, "str_literal");

    //return llvm::GetElementPtrConstantExpr(val->getType(),
    //    val,
    //    std::vector<llvm::Constant*> { zero },
    //    cstrType,
    //    0);
}

void FuncCallNode::Process()
{
    ConvertToOnlyCustom();
    CustomNode::Process();
}

llvm::Value* FuncCallNode::Evaluate(CompileContext& context)
{
    std::cout << "***** EVALUATED *****" << std::endl;

    // TODO: make robust
    auto funcNameNode = PtrCast<IdentifierNode>(customChildren[0]);
    std::string funcName = funcNameNode->GetValue();

    auto exprNode = PtrCast<ExpressionNode>(customChildren[1]);
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


