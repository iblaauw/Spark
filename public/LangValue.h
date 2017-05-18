#pragma once

#include "llvm/IR/Value.h"

#include "LangType.h"
#include "PtrUtils.h"

class RValue
{
public:
    virtual bool IsLValue() const { return false; }
    virtual llvm::Value* GetValue() const = 0;
    virtual Ptr<LangType> GetType() const = 0;
};

class LValue : public RValue
{
public:
    virtual bool IsLValue() const override { return true; }
    virtual void Assign(const RValue& value, CompileContext& context) const;
};

class GeneralRValue : public RValue
{
private:
    llvm::Value* value;
    Ptr<LangType> type;
public:
    GeneralRValue(llvm::Value* value, Ptr<LangType> type);

    llvm::Value* GetValue() const override { return value; }
    Ptr<LangType> GetType() const override { return type; }
};

