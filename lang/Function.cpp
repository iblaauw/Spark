#include "Function.h"

Function::Function(std::string name,
        Ptr<LangType> returnType,
        const std::vector<Ptr<LangType>>& parameterTypes,
        const std::vector<std::string>& parameterNames) :
        name(name), returnType(returnType), parameterTypes(parameterTypes), parameterNames(parameterNames)
{}

void Function::GetIRTypes(std::vector<llvm::Type*>& typesOut) const
{
    typesOut.clear();
    for (Ptr<LangType> type : parameterTypes)
    {
        typesOut.push_back(type->GetIR());
    }
}


