#include "LangType.h"

#include "llvm/IR/DerivedTypes.h"

BasicType::BasicType(std::string name) : name(name) {}

bool BasicType::IsAssignableFrom(LangType* otherType) const
{
    if (otherType->IsPointer())
        return false;
    return name == otherType->GetName();
}

void BasicType::InsertConversion(LangType* fromType, CompileContext& context) const
{
    // NOOP (default will not need to convert)
}

PointerType::PointerType(LangType* subType) : subType(subType) {}

llvm::Type* PointerType::GetIR() const
{
    llvm::Type* type = subType->GetIR()->getPointerTo();
    return type;
}

bool PointerType::IsAssignableFrom(LangType* otherType) const
{
    if (!otherType->IsPointer())
        return false;

    PointerType* other = static_cast<PointerType*>(otherType);
    return other->subType == this->subType;
}

void PointerType::InsertConversion(LangType* fromType, CompileContext& context) const
{
    // NOOP
}

ArrayType::ArrayType(LangType* subType, int size) : subType(subType), size(size) {}

llvm::Type* ArrayType::GetIR() const
{
    llvm::Type* sub = subType->GetIR();
    return llvm::ArrayType::get(sub, size);
}

bool ArrayType::IsAssignableFrom(LangType* otherType) const
{
    if (!otherType->IsArray())
        return false;

    ArrayType* other = static_cast<ArrayType*>(otherType);
    if (other->subType != this->subType)
        return false;

    return this->size <= other->size;
}

void ArrayType::InsertConversion(LangType* fromType, CompileContext& context) const
{
    // NOOP
}



