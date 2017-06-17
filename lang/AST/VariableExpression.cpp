#include "AST/VariableExpression.h"

#include <iostream>

#include "AST/Common.h"
#include "Variable.h"

RULE(VariableExpression)
{
    Autoname(builder);
    builder.Add(Identifier);

    builder.SetNodeType<VariableExpressionNode>();
}


UnknownPtr<RValue> VariableExpressionNode::Evaluate(CompileContext& context)
{
    auto identifier = SafeGet<IdentifierNode>(0, "IdentifierNode");
    std::string varname = identifier->GetValue();

    LValue* var = context.symbolTable->variables.Get(varname);
    if (var != nullptr)
        return var;

    Function* func = context.symbolTable->functions.Get(varname);

    if (func != nullptr)
    {
        auto result = std::make_shared<GeneralRValue>(func->GetIR(), func->GetFuncType());
        return PtrCast<RValue>(result);
    }

    Error("no variable named '", varname, "' exists");
    return nullptr;
}

