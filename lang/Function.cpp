#include "Function.h"

Function::Function(std::string name,
        LangType* returnType,
        const std::vector<LangType*>& parameterTypes,
        const std::vector<std::string>& parameterNames) :
        name(name), returnType(returnType), parameterTypes(parameterTypes), parameterNames(parameterNames)
{}

void Function::GetIRTypes(std::vector<llvm::Type*>& typesOut) const
{
    typesOut.clear();
    for (LangType* type : parameterTypes)
    {
        typesOut.push_back(type->GetIR());
    }
}


