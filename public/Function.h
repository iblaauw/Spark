#pragma once

#include <vector>
#include <string>

#include "LangType.h"
#include "PtrUtils.h"
#include "llvm/IR/Function.h"

class Function
{
private:
    std::string name;
    Ptr<LangType> returnType;
    std::vector<Ptr<LangType>> parameterTypes;
    std::vector<std::string> parameterNames;
    llvm::Function* func;
public:
    Function(std::string name,
            Ptr<LangType> returnType,
            const std::vector<Ptr<LangType>>& parameterTypes,
            const std::vector<std::string>& parameterNames);

    std::string GetName() const { return name; }

    Ptr<LangType> ReturnType() const { return returnType; }

    const std::vector<Ptr<LangType>>& ParameterTypes() { return parameterTypes; }

    const std::vector<std::string>& ParameterNames() { return parameterNames; }

    llvm::Function* GetIR() const { return func; }
    void SetIR(llvm::Function* func) { this->func = func; }

    void GetIRTypes(std::vector<llvm::Type*>& typesOut) const;
};
