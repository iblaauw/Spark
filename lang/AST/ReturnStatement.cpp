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
    if (context.currentFunction == nullptr)
    {
        std::cerr << "Internal Error: Return statement not inside a function" << std::endl;
        return;
    }

    LangType* retType = context.currentFunction->ReturnType();

    size_t size = customChildren.size();

    if (size > 1)
    {
        std::cerr << "Internal Error: Return statement with multiple expressions" << std::endl;
        return;
    }

    LangType* voidType = context.symbolTable.types.Get("void");

    if (retType == voidType)
    {
        if (size == 1)
        {
            std::cerr << "Error: a function with 'void' return type cannot return a value" << std::endl;
            return;
        }

        context.builder.CreateRetVoid();
        return;
    }

    if (size == 0)
    {
        std::cerr << "Error: empty return statement in function that returns '" <<
            retType->GetName() << "'" << std::endl;
        return;
    }

    auto result = customChildren[0]->Evaluate(context);
    LangType* exprType = result->GetType();
    if (!retType->IsAssignableFrom(*exprType))
    {
        std::cerr << "Error: cannot return a value of type '" << exprType->GetName()
            << "' in function that returns '" << retType->GetName() << "'" << std::endl;
        return;
    }

    context.builder.CreateRet(result->GetValue());
}


