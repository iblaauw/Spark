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

    virtual void Assign(llvm::Value* value, CompileContext& context) const = 0;
    virtual llvm::Value* GetAddress(CompileContext& context) = 0;
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

class PointerLValue : public LValue
{
private:
    llvm::Value* ptr;
    LangType* type;
public:
    PointerLValue(llvm::Value* value, LangType* type);

    llvm::Value* GetValue(CompileContext& context) const override;
    LangType* GetType() const override { return type; }

    void Assign(llvm::Value* value, CompileContext& context) const override;
    llvm::Value* GetAddress(CompileContext& context) override { return ptr; }
};


