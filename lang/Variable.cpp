#include "Variable.h"

#include <iostream>

#include "CompileContext.h"
#include "Errors.h"

//RegisterVariable::RegisterVariable(std::string name, LangType* type) : Variable(name, type),
//    updater()
//{
//    updater.Initialize(type->GetIR(), name);
//}
//
//
//void RegisterVariable::SetValue(llvm::Value* value)
//{
//    Assert(value != nullptr, "null variable IR value");
//    Assert(type->GetIR() == value->getType(), "mismatched variable IR type in RegisterVariable");
//
//    baseValue = value;
//}
//
//llvm::Value* RegisterVariable::GetValue(CompileContext& context) const
//{
//    llvm::BasicBlock* currentBB = context.builder.GetInsertBlock();
//    return updater.GetValueAtEndOfBlock(currentBB);
//}
//
//
//void RegisterVariable::Assign(llvm::Value* value, CompileContext& context) const
//{
//    Assert(value != nullptr, "null argument");
//    Assert(value->getType() == type->GetIR(), "type mismatch for assigning to register variable");
//
//    llvm::BasicBlock* currentBB = context.builder.GetInsertBlock();
//
//    updater.AddAvailableValue(currentBB, value);
//}
//
//llvm::Value* RegisterVariable::GetAddress(CompileContext& context)
//{
//    Assert(false, "cannot get address of a RegisterVariable");
//    return nullptr;
//}
//
//void RegisterVariable::Allocate(CompileContext& context)
//{
//    Assert(baseValue != nullptr, "RegisterValue never had 'SetValue' called");
//
//    llvm::BasicBlock* currentBB = context.builder.GetInsertBlock();
//    updater.AddAvailableValue(currentBB, baseValue);
//    baseValue = nullptr;
//}
//
//void RegisterVariable::Initialize(CompileContext& context)
//{
//    // Do nothing
//}


MemoryVariable::MemoryVariable(std::string name, LangType* type) : Variable(name, type)
{}

llvm::Value* MemoryVariable::GetValue(CompileContext& context) const
{
    Assert(ptr != nullptr, "null pointer in MemoryVariable");

    return context.builder.CreateLoad(ptr, name + "_val");
}

void MemoryVariable::Assign(llvm::Value* value, CompileContext& context) const
{
    Assert(value != nullptr, "null argument");
    Assert(ptr != nullptr, "no pointer initialized for MemoryVariable");
    Assert(value->getType()->getPointerTo() == ptr->getType(), "type mismatch for assigning to memory variable");

    context.builder.CreateStore(value, ptr);
}

llvm::Value* MemoryVariable::GetAddress(CompileContext& context) { return ptr; }

void MemoryVariable::Allocate(CompileContext& context)
{
    Assert(type != nullptr, "null type in MemoryVariable");
    llvm::Value* val = context.builder.CreateAlloca(type->GetIR(), nullptr, name);
    this->ptr = val;
}

void MemoryVariable::Initialize(CompileContext& context)
{
    // Nothing (for now, wait until classes are in place)
}



ParameterVariable::ParameterVariable(std::string name, LangType* type)
    : MemoryVariable(name, type), param(nullptr)
{}

void ParameterVariable::SetValue(llvm::Value* value)
{
    Assert(value != nullptr, "null variable IR value");
    Assert(type->GetIR() == value->getType(), "mismatched variable IR type in RegisterVariable");

    param = value;
}

void ParameterVariable::Initialize(CompileContext& context)
{
    Assign(param, context);
}


