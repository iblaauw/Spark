#pragma once

#include "OperatorImpl.h"
#include "UnknownPtr.h"
#include "LangValue.h"

using RValuePtr = UnknownPtr<RValue>;

class ArrayIndexOperator : public SpecialOperatorImpl
{
public:
    RValuePtr Create(RValuePtr lhs, std::vector<RValuePtr>& args, CompileContext& context) override;
};

void AddIntOperators(LangType* intType);
void AddBoolOperators(LangType* boolType);



