#include "Function.h"
#include "LLVMManager.h"

Function::Function(std::string name,
        LangType* returnType,
        const std::vector<LangType*>& parameterTypes,
        const std::vector<std::string>& parameterNames) :
        name(name), returnType(returnType), parameterTypes(parameterTypes), parameterNames(parameterNames)
{}

void Function::SetIRDefault()
{
    auto& manager = Spark::LLVMManager::Instance();

    std::vector<llvm::Type*> argsIR;
    GetIRTypes(argsIR);
    auto sig = manager.GetFuncSignature(returnType->GetIR(), argsIR);
    llvm::Function* funcIR = manager.DeclareFunction(name, sig);
    SetIR(funcIR);
}

void Function::GetIRTypes(std::vector<llvm::Type*>& typesOut) const
{
    typesOut.clear();
    for (LangType* type : parameterTypes)
    {
        typesOut.push_back(type->GetIR());
    }
}


