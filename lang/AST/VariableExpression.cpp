#include "AST/VariableExpression.h"

#include <iostream>

#include "AST/Common.h"

RULE(VariableExpression)
{
    Autoname(builder);
    builder.Add(Identifier);

    builder.SetNodeType<VariableExpressionNode>();
}


Ptr<RValue> VariableExpressionNode::Evaluate(CompileContext& context)
{
    auto identifier = SafeGet<IdentifierNode>(0, "IdentifierNode");

    if (identifier == nullptr)
    {
        std::cerr << "Internal Error: Invalid name for variable expression." << std::endl;
        return nullptr;
    }

    std::string varname = identifier->GetValue();

    Ptr<Variable> var = context.symbolTable.variables.Get(varname);
    if (var == nullptr)
    {
        std::cerr << "Error: no variable named '" << varname << "' exists" << std::endl;
        return nullptr;
    }

    return PtrCast<RValue>(var);
}

