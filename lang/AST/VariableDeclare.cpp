#include "AST/VariableDeclare.h"

#include "AST/Common.h"

RULE(VariableDeclaration)
{
    Autoname(builder);
    builder.Add(Type, Whitespace, Identifier, ';');
    builder.Ignore(1);
    builder.Ignore(3);
}


void VariableDeclareNode::GatherSymbols(CompileContext& context)
{
    auto typeNode = SafeGet<TypeNode>(0, "TypeNode");
    if (typeNode == nullptr)
    {
        std::cerr << "Internal Error: invalid type node in VariableDeclaration" << std::endl;
        return;
    }

    auto idNode = SafeGet<IdentifierNode>(1, "IdentifierNode");
    if (idNode == nullptr)
    {
        std::cerr << "Internal Error: invalid identifier node in VariableDeclaration" << std::endl;
        return;
    }

    LangType* type = typeNode->GetIRType();
    std::string name = idNode->GetValue();

    variable = new MemoryVariable(name, type);
    context.symbolTable->variables.Add(name, variable);
}

void VariableDeclareNode::Generate(CompileContext& context)
{
    llvm::Value* val = context.builder.CreateAlloca(variable->GetType()->GetIR(), nullptr, variable->GetName());
    variable->SetValue(val);
}


