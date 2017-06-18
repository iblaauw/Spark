#include "MemberValue.h"

#include "CompileContext.h"
#include "TypeConverter.h"

DataMemberValue::DataMemberValue(std::string name, unsigned int memberindex, LangType* memberType)
    : index(memberindex), type(memberType), name(name)
{}

UnknownPtr<RValue> DataMemberValue::GetMember(UnknownPtr<LValue> thisPtr, CompileContext& context)
{
    llvm::Value* ptr = thisPtr->GetAddress(context);

    auto zero = Spark::TypeConverter::Create<int>(0);
    auto index_val = Spark::TypeConverter::Create<int>(index);
    std::vector<llvm::Value*> indices { zero, index_val };
    llvm::Value* memberPtr = context.builder.CreateGEP(ptr, indices, name+"_ptr");
    auto final_val = std::make_shared<PointerLValue>(memberPtr, type);
    return PtrCast<RValue>(final_val);
}


