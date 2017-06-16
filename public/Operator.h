#pragma once

#include "OperatorImpl.h"
#include "UnknownPtr.h"
#include "LangValue.h"

class ArrayIndexOperator : public SpecialOperatorImpl
{
public:
    UnknownPtr<RValue> Create(UnknownPtr<RValue> lhs, std::vector<UnknownPtr<RValue>>& args, CompileContext& context) override;
};

