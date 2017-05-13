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

void FuncCallNode::Process()
{
    ConvertToOnlyCustom();
    CustomNode::Process();
}


