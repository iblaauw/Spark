#include "LangType.h"

#include "llvm/IR/DerivedTypes.h"

#include "TypeConverter.h"
#include "CompileContext.h"
#include "Operator.h"

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

PointerType::PointerType(LangType* subType) : subType(subType)
{
    members.unaryOperators["*"] = &(DereferenceOperator::Instance());
}

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

ArrayType::ArrayType(llvm::Type* irType, LangType* subType, int size)
    : type(irType), subType(subType), size(size)
{
    members.indexOperator = &(ArrayIndexOperator::Instance());
}

llvm::Type* ArrayType::GetIR() const
{
    return type;
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

void ArrayType::CallConstructor(UnknownPtr<LValue> lval,
                                std::vector<UnknownPtr<RValue>>& arguments,
                                CompileContext& context)
{
    llvm::Type* arrayType = llvm::ArrayType::get(subType->GetIR(), size);
    auto data = context.builder.CreateAlloca(arrayType);

    auto zero = Spark::TypeConverter::Create<int>(0);
    auto ptr1 = context.builder.CreateGEP(data,
        std::vector<llvm::Value*>{ zero, zero });

    auto sizeVal = Spark::TypeConverter::Create<int>(size);

    llvm::Value* undefVal = llvm::UndefValue::get(this->type);
    auto part1 = context.builder.CreateInsertValue(undefVal, sizeVal, std::vector<unsigned>{ 0 });
    auto part2 = context.builder.CreateInsertValue(part1, ptr1, std::vector<unsigned>{ 1 });

    lval->Assign(part2, context);
}

/*static*/ llvm::Type* ArrayType::CreateMasterType(LangType* subType)
{
    llvm::Type* ptrType = subType->GetIR()->getPointerTo();
    llvm::Type* intType = Spark::TypeConverter::Get<int>();

    llvm::Type* types[2] = { intType, ptrType };
    return llvm::StructType::create(types, subType->GetName() + "_array_t");
}



