#include "Variable.h"

#include <iostream>

#include "CompileContext.h"

Variable::Variable(std::string name, Ptr<LangType> type) : name(name), type(type)
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


