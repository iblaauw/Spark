#include "AST/VariableDeclare.h"

#include "AST/Common.h"

RULE(VariableDeclaration)
{
    Autoname(builder);
    builder.Add(Type, Whitespace, Identifier, ';');
    builder.Ignore(1);
    builder.Ignore(3);

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
    // These are now handled by function allocation
    //llvm::Value* val = context.builder.CreateAlloca(variable->GetType()->GetIR(), nullptr, variable->GetName());
    //variable->SetValue(val);
}


