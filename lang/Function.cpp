#include "Function.h"
#include "LLVMManager.h"
#include "Variable.h"

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

void Function::RegisterForAllocation(Variable* var)
{
    allocationSet.push_back(var);
}

void Function::AllocateAndInitialize(CompileContext& context)
{
    for (Variable* var : allocationSet)
    {
        var->Allocate(context);
    }

    for (Variable* var : allocationSet)
    {
        var->Initialize(context);
    }

    allocationSet.clear();
}

