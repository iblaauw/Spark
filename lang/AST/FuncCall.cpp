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

Ptr<RValue> FuncCallNode::Evaluate(CompileContext& context)
{
    // TODO: make robust
    auto funcNameNode = PtrCast<IdentifierNode>(customChildren[0]);
    std::string funcName = funcNameNode->GetValue();

    auto exprNode = customChildren[1];
    Ptr<RValue> result = exprNode->Evaluate(context);

    if (result == nullptr)
    {
        auto error = std::make_shared<ErrorValue>();
        return PtrCast<RValue>(error);
    }

    llvm::Function* func = context.symbolTable.GetFunction(funcName);

    if (func == nullptr)
    {
        std::cerr << "Error: a function named '" << funcName << "' could not be found." << std::endl;
        auto error = std::make_shared<ErrorValue>();
        return PtrCast<RValue>(error);
    }

    std::vector<llvm::Value*> args { result->GetValue() };
    llvm::Value* value = context.builder.CreateCall(func, args, funcName + "_call");
    LangType* retType = context.symbolTable.GetType("void");

    auto ptrVal = std::make_shared<GeneralRValue>(value, retType);
    return PtrCast<RValue>(ptrVal);
}


