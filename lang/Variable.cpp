#include "Variable.h"

#include <iostream>

Variable::Variable(std::string name, const LangType& type) : name(name), type(type)
{}

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


