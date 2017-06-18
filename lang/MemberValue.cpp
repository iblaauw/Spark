#include "MemberValue.h"

#include "CompileContext.h"
#include "TypeConverter.h"

DataMemberValue::DataMemberValue(std::string name, unsigned int memberindex, LangType* memberType)
    : index(memberindex), type(memberType), name(name)
{}

UnknownPtr<RValue> DataMemberValue::GetMember(UnknownPtr<RValue> thisPtr, CompileContext& context)
{
    if (thisPtr->IsLValue())
    {
        auto lval = thisPtr.Cast<LValue>();
        llvm::Value* ptr = lval->GetAddress(context);

        auto zero = Spark::TypeConverter::Create<int>(0);
        auto index_val = Spark::TypeConverter::Create<int>(index);
        std::vector<llvm::Value*> indices { zero, index_val };
        llvm::Value* memberPtr = context.builder.CreateGEP(ptr, indices, name+"_ptr");
        auto final_val = std::make_shared<PointerLValue>(memberPtr, type);
        return PtrCast<RValue>(final_val);
    }
    else
    {
        llvm::Value* val = thisPtr->GetValue(context);
        std::vector<unsigned int> indices { index };
        llvm::Value* member = context.builder.CreateExtractValue(val, indices, name+"_ptr");
        auto final_val = std::make_shared<GeneralRValue>(member, type);
        return PtrCast<RValue>(final_val);
    }
}


