#include "LangValue.h"

#include <iostream>

GeneralRValue::GeneralRValue(llvm::Value* value, Ptr<LangType> type)
    : value(value), type(type)
{
    if (value->getType() != type->GetIR())
    {
        std::cerr << "Warning: mismatched type on GeneralRValue" << std::endl;
    }
}

