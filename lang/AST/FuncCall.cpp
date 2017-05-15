#include "AST/FuncCall.h"

#include <iostream>

#include "AST/Common.h"
#include "AST/Expression.h"

RULE(FuncCall)
{
    Autoname(builder);
    builder.Add(Identifier, '(', Expression, ')');

    builder.SetNodeType<FuncCallNode>();
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


