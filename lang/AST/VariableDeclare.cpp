#include "AST/VariableDeclare.h"

#include "AST/Common.h"
#include "AST/Type.h"
#include "AST/Expression.h"

RULE(VariableDeclaration)
{
    Autoname(builder);
    builder.Add(Type, Whitespace, Identifier, OptionalWhitespace, '=', OptionalWhitespace, ExpressionTree);
    builder.Add(Type, Whitespace, Identifier);

    builder.Ignore(1);
    builder.Ignore(3);
    builder.Ignore(5);

    builder.SetNodeType<VariableDeclareNode>();
}


void VariableDeclareNode::GatherSymbols(CompileContext& context)
{
    auto typeNode = SafeGet<TypeNode>(0, "TypeNode");
    auto idNode = SafeGet<IdentifierNode>(1, "IdentifierNode");

    LangType* type = typeNode->GetIRType();
    std::string name = idNode->GetValue();

    variable = new MemoryVariable(name, type);
    context.symbolTable->variables.Add(name, variable);

    Assert(context.currentFunction != nullptr, "no current function found");

    context.currentFunction->RegisterForAllocation(variable);
}

void VariableDeclareNode::Generate(CompileContext& context)
{
    LangType* ltype = variable->GetType();

    if (customChildren.size() <= 2)
    {
        std::vector<UnknownPtr<RValue>> args {};
        ltype->CallConstructor(variable, args, context);
        return;
    }

    UnknownPtr<RValue> rhs = customChildren[2]->Evaluate(context);
    if (rhs == nullptr)
        return;

    LangType* rtype = rhs->GetType();
    if (!ltype->IsAssignableFrom(rtype))
    {
        Error("cannot assign a value of type '", rtype->GetName(), "' to l-value of type '", ltype->GetName(), "'");
        return;
    }

    auto rval = rhs->GetValue(context);
    variable->Assign(rval, context);
}


