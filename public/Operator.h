#pragma once

#include "OperatorImpl.h"
#include "UnknownPtr.h"
#include "LangValue.h"

using RValuePtr = UnknownPtr<RValue>;

class ArrayIndexOperator : public SpecialOperatorImpl, public Singleton<ArrayIndexOperator>
{
public:
    RValuePtr Create(RValuePtr lhs, std::vector<RValuePtr>& args, CompileContext& context) override;
};

class DereferenceOperator : public UnaryOperatorImpl, public Singleton<DereferenceOperator>
{
public:
    RValuePtr Create(RValuePtr rhs, CompileContext& context) override;
};

class AddressOfOperator : public UnaryOperatorImpl, public Singleton<AddressOfOperator>
{
public:
    RValuePtr Create(RValuePtr rhs, CompileContext& context) override;
};

class FuncCallOperator : public SpecialOperatorImpl, public Singleton<FuncCallOperator>
{
public:
    RValuePtr Create(RValuePtr lhs, std::vector<RValuePtr>& args, CompileContext& context) override;
private:
    bool IsCompatible(FunctionType* funcType, std::vector<RValuePtr>& args);
};

void AddIntOperators(LangType* intType);
void AddBoolOperators(LangType* boolType);



