#pragma once

#include "llvm/IR/Value.h"

#include "LangType.h"
#include "PtrUtils.h"

class CompileContext;

class RValue
{
public:
    virtual bool IsLValue() const { return false; }
    virtual llvm::Value* GetValue(CompileContext& context) const = 0;
    virtual LangType* GetType() const = 0;
    virtual ~RValue() {}
};

class LValue : public RValue
{
public:
    virtual bool IsLValue() const override { return true; }
    virtual void Assign(const RValue& value, CompileContext& context) const = 0;
};

class GeneralRValue : public RValue
{
private:
    llvm::Value* value;
    LangType* type;
public:
    GeneralRValue(llvm::Value* value, LangType* type);

    llvm::Value* GetValue(CompileContext& context) const override { return value; }
    LangType* GetType() const override { return type; }
};

