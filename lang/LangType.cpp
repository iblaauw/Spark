#include "LangType.h"

#include "llvm/IR/DerivedTypes.h"

Ptr<LangType> LangType::GetPointerTo()
{
    return std::shared_ptr<LangType>(new PointerType(this));
}

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


