#include "Variable.h"

#include <iostream>

#include "CompileContext.h"

Variable::Variable(std::string name, LangType* type) : name(name), type(type)
{}


void Variable::SetValue(llvm::Value* value)
{
    this->value = value;
    if (value == nullptr)
    {
        std::cerr << "Internal Error: null variable IR value" << std::endl;
        return;
    }

    if (type->GetIR() != value->getType())
    {
        std::cerr <<  "Warning: mismatched variable types" << std::endl;
    }
}

void Variable::Assign(const RValue& newValue, CompileContext& context) const
{
    if (value == nullptr)
    {
        std::cerr << "Internal Error: Variable has no IR value" << std::endl;
        return;
    }

    // TODO: do this
    std::cerr << "Error: Not Implemented" << std::endl;
}

MemoryVariable::MemoryVariable(std::string name, LangType* type) : name(name), type(type)
{}

void MemoryVariable::SetValue(llvm::Value* value)
{
    if (value == nullptr)
    {
        std::cerr << "Internal Error: argument null in MemoryVariable" << std::endl;
        return;
    }

    llvm::Type* ptrType = type->GetIR()->getPointerTo();
    if (value->getType() != ptrType)
    {
        std::cerr << "Internal Error: mismatched variable type in MemoryVariable" << std::endl;
        return;
    }

    ptr = value;
}

llvm::Value* MemoryVariable::GetValue(CompileContext& context) const
{
    if (ptr == nullptr)
    {
        std::cerr << "Internal Error: null pointer in MemoryVariable" << std::endl;
        return nullptr;
    }

    return context.builder.CreateLoad(ptr, name + "_val");
}

void MemoryVariable::Assign(const RValue& newValue, CompileContext& context) const
{
    LangType* newType = newValue.GetType();
    if (!type->IsAssignableFrom(*newType))
    {
        std::cerr << "Error: cannot assign a value of type '" << newType->GetName() <<
            "' to variable '" << name << "' with type '" << type->GetName() << "'"
            << std::endl;
        return;
    }

    // TODO: add implicit conversions

    context.builder.CreateStore(ptr, newValue.GetValue(context));
}



