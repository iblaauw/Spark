#include "Variable.h"

#include <iostream>

#include "CompileContext.h"
#include "Errors.h"

RegisterVariable::RegisterVariable(std::string name, LangType* type) : Variable(name, type),
    updater()
{
    updater.Initialize(type->GetIR(), name);
}


void RegisterVariable::SetValue(llvm::Value* value)
{
    Assert(value != nullptr, "null variable IR value");
    Assert(type->GetIR() == value->getType(), "mismatched variable IR type in RegisterVariable");

    baseValue = value;
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
        Error("cannot assign a value of type '", valType->GetName(), "' to variable '", name, "' with type '", type->GetName(), "'");
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
    Assert(ptr != nullptr, "null pointer in MemoryVariable");

    return context.builder.CreateLoad(ptr, name + "_val");
}

void MemoryVariable::Assign(const RValue& newValue, CompileContext& context) const
{
    LangType* newType = newValue.GetType();
    if (!type->IsAssignableFrom(*newType))
    {
        Error("cannot assign a value of type '", newType->GetName(), "' to variable '", name, "' with type '", type->GetName(), "'");
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


