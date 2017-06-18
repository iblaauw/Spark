#include "LangValue.h"

#include <iostream>

#include "CompileContext.h"
#include "Errors.h"

GeneralRValue::GeneralRValue(llvm::Value* value, LangType* type)
    : value(value), type(type)
{
    if (value->getType() != type->GetIR() && !type->IsFunction())
    {
        std::cerr << "Warning: mismatched type on GeneralRValue (expected " << type->GetName() << ")" << std::endl;
    }
}

PointerLValue::PointerLValue(llvm::Value* ptr, LangType* type)
    : ptr(ptr), type(type)
{
    Assert(ptr != nullptr, "null argument");
    Assert(type != nullptr, "null argument");

    llvm::PointerType* ptrType = llvm::dyn_cast<llvm::PointerType>(ptr->getType());
    Assert(ptrType != nullptr, "a non-pointer was given to PointerLValue");

    if (ptrType != type->GetIR()->getPointerTo())
    {
        std::cerr << "Warning: mismatched type on PointerLValue" << std::endl;
    }
}

llvm::Value* PointerLValue::GetValue(CompileContext& context) const
{
    Assert(ptr != nullptr, "pointer l-value not initialized");
    return context.builder.CreateLoad(ptr);
}

void PointerLValue::Assign(llvm::Value* value, CompileContext& context) const
{
    Assert(value != nullptr, "null argument");
    Assert(value->getType() == type->GetIR(), "type mismatch for assigning to pointer variable");
    context.builder.CreateStore(value, ptr);
}

