#pragma once

#include <vector>
#include <string>

#include "LangType.h"
#include "PtrUtils.h"
#include "llvm/IR/Function.h"

class Variable;

class Function
{
private:
    std::string name;
    LangType* returnType;
    std::vector<LangType*> parameterTypes;
    std::vector<std::string> parameterNames;
    llvm::Function* func;
    std::vector<Variable*> allocationSet;
public:

    Function(std::string name,
            LangType* returnType,
            const std::vector<LangType*>& parameterTypes,
            const std::vector<std::string>& parameterNames);

    std::string GetName() const { return name; }

    LangType* ReturnType() const { return returnType; }

    const std::vector<LangType*>& ParameterTypes() { return parameterTypes; }

    const std::vector<std::string>& ParameterNames() { return parameterNames; }

    llvm::Function* GetIR() const { return func; }
    void SetIR(llvm::Function* func) { this->func = func; }
    void SetIRDefault();

    void GetIRTypes(std::vector<llvm::Type*>& typesOut) const;

    void RegisterForAllocation(Variable* var);
    void AllocateAndInitialize(CompileContext& context);
};
