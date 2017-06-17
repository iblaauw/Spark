#include "Function.h"
#include "LLVMManager.h"
#include "Variable.h"
#include "Errors.h"

Function::Function(std::string name,
        FunctionType* funcType,
        const std::vector<std::string>& parameterNames) :
        name(name), funcType(funcType), parameterNames(parameterNames)
{
    Assert(funcType->ParameterTypes().size() == parameterNames.size(), "Mismatched parameters  and parameter names in function declarations.");
}

void Function::SetIRDefault()
{
    auto& manager = Spark::LLVMManager::Instance();

    llvm::Function* funcIR = manager.DeclareFunction(name, funcType->GetFuncIR());
    SetIR(funcIR);
}

void Function::GetIRTypes(std::vector<llvm::Type*>& typesOut) const
{
    typesOut.clear();
    for (LangType* type : funcType->ParameterTypes())
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

