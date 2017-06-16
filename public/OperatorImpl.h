#pragma once

#include <vector>

#include "UnknownPtr.h"

class RValue;
class CompileContext;

class UnaryOperatorImpl
{
public:
    virtual UnknownPtr<RValue> Create(UnknownPtr<RValue> lhs, CompileContext& context) = 0;
};

class BinaryOperatorImpl
{
public:
    virtual UnknownPtr<RValue> Create(UnknownPtr<RValue> lhs, UnknownPtr<RValue> rhs, CompileContext& context) = 0;
};

class SpecialOperatorImpl
{
public:
    virtual UnknownPtr<RValue> Create(UnknownPtr<RValue> lhs, std::vector<UnknownPtr<RValue>>& args, CompileContext& context) = 0;
};

