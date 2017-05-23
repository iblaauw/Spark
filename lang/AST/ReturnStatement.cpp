#include "AST/ReturnStatement.h"

#include <iostream>

#include "AST/Common.h"
#include "AST/Expression.h"

RULE(ReturnStatement)
{
    Autoname(builder);
    builder.Add("return", Whitespace, ExpressionTree, OptionalWhitespace, ';');
    builder.Add("return", OptionalWhitespace, ';');

    builder.Ignore(0);
    builder.Ignore(1);
    builder.Ignore(3);
    builder.Ignore(4);

    builder.SetNodeType<ReturnStatementNode>();
}

void ReturnStatementNode::Process()
{
    ConvertToOnlyCustom();

    CustomNode::Process();
}

void ReturnStatementNode::Generate(CompileContext& context)
{
    Assert(context.currentFunction != nullptr, "return statement not inside a function");

    LangType* retType = context.currentFunction->ReturnType();

    size_t size = customChildren.size();

    Assert(size <= 1, "return statement with multiple expressions");

    LangType* voidType = context.symbolTable->types.Get("void");

    if (retType == voidType)
    {
        if (size == 1)
        {
            Error("a function with a 'void' return type cannot return a value");
            return;
        }

        context.builder.CreateRetVoid();
        return;
    }

    if (size == 0)
    {
        Error("empty return statement in function that returns '", retType->GetName(), "'");
        return;
    }

    auto result = customChildren[0]->Evaluate(context);
    LangType* exprType = result->GetType();
    if (!retType->IsAssignableFrom(*exprType))
    {
        Error("cannot return a value of type '", exprType->GetName(), "' in function that returns '", retType->GetName(), "'");
        return;
    }

    auto* to_return = result->GetValue(context);
    context.builder.CreateRet(to_return);
}


