#include "AST.h"
#include "LLVMManager.h"
#include "TypeConverter.h"

void FunctionNode::Process()
{
    NodePtr nameIdentifier = children[1];

    children.clear();
    children.push_back(nameIdentifier);

    NodePtr strNode = (*nameIdentifier)[0];
    auto actualNode = PtrCast<Spark::StringNode>(strNode);
    funcName = actualNode->Get();
}

void FunctionNode::Generate()
{
    auto& manager = Spark::LLVMManager::Instance();

    llvm::Type* voidType = Spark::TypeConverter::Get<void>();
    auto signature = manager.GetFuncSignature(voidType);
    llvm::Function* func = manager.DeclareFunction(funcName, signature);

    llvm::IRBuilder<> builder = manager.Implement(func);
    builder.CreateRetVoid();

    // TODO: recursively use the new builder
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


