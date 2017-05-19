#include "Variable.h"

#include <iostream>

#include "CompileContext.h"

Variable::Variable(std::string name, LangType* type) : name(name), type(type)
{}


void Variable::SetValue(llvm::Value* value)
{
    this->value = value;
    if (value == nullptr)
    {
        std::cerr << "Internal Error: null variable IR value" << std::endl;
        return;
    }

    if (type->GetIR() != value->getType())
    {
        std::cerr <<  "Warning: mismatched variable types" << std::endl;
    }
}

void Variable::Assign(const RValue& newValue, CompileContext& context) const
{
    if (value == nullptr)
    {
        std::cerr << "Internal Error: Variable has no IR value" << std::endl;
        return;
    }

    // TODO: do this
    std::cerr << "Error: Not Implemented" << std::endl;
}


