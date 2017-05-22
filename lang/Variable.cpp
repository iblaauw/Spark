#include "Variable.h"

#include <iostream>

#include "CompileContext.h"

RegisterVariable::RegisterVariable(std::string name, LangType* type) : Variable(name, type),
    updater()
{
    updater.Initialize(type->GetIR(), name);
}


void RegisterVariable::SetValue(llvm::Value* value)
{
    if (value == nullptr)
    {
        std::cerr << "Internal Error: null variable IR value" << std::endl;
        return;
    }

    if (type->GetIR() != value->getType())
    {
        std::cerr <<  "Internal Error: mismatched variable type in RegisterVariable" << std::endl;
        return;
    }

    baseValue = value;
    //updater.AddAvailableValue(currentBB, value);
}

llvm::Value* RegisterVariable::GetValue(CompileContext& context) const
{
    llvm::BasicBlock* currentBB = context.builder.GetInsertBlock();
    return updater.GetValueAtEndOfBlock(currentBB);
}


void RegisterVariable::Assign(const RValue& newValue, CompileContext& context) const
{
    llvm::Value* val = newValue.GetValue(context);

    // Note: its important to get the value before the block (getting the value inserts instructions)
    llvm::BasicBlock* currentBB = context.builder.GetInsertBlock();

    LangType* valType = newValue.GetType();
    if (!type->IsAssignableFrom(*valType))
    {
        std::cerr << "Error: cannot assign a value of type '" << valType->GetName() << "' to variable '" << name << "' of type '" << type->GetName() << "'" << std::endl;
        return;
    }

    updater.AddAvailableValue(currentBB, val);
}

void RegisterVariable::Allocate(CompileContext& context)
{
    llvm::BasicBlock* currentBB = context.builder.GetInsertBlock();
    updater.AddAvailableValue(currentBB, baseValue);
    baseValue = nullptr;
}

void RegisterVariable::Initialize(CompileContext& context)
{
    // Do nothing
}


MemoryVariable::MemoryVariable(std::string name, LangType* type) : Variable(name, type)
{}

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

    context.builder.CreateStore(newValue.GetValue(context), ptr);
}

void MemoryVariable::Allocate(CompileContext& context)
{
    llvm::Value* val = context.builder.CreateAlloca(type->GetIR(), nullptr, name);
    this->ptr = val;
}

void MemoryVariable::Initialize(CompileContext& context)
{
    // Nothing (for now, wait until classes are in place)
}


