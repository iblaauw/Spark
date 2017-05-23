#pragma once

#include "llvm/Transforms/Utils/SSAUpdater.h"

#include "LangType.h"
#include "LangValue.h"

class Variable : public LValue
{
protected:
    std::string name;
    LangType* type;
public:
    Variable(std::string name, LangType* type) : name(name), type(type) {}

    std::string GetName() const { return name; }
    LangType* GetType() const override { return type; }

    virtual void Allocate(CompileContext& context) = 0;
    virtual void Initialize(CompileContext& context) = 0;
};

class RegisterVariable : public Variable
{
    mutable llvm::SSAUpdater updater;
    llvm::Value* baseValue;
public:
    RegisterVariable(std::string name, LangType* type);

    void SetValue(llvm::Value* value);

    llvm::Value* GetValue(CompileContext& context) const override;
    void Assign(llvm::Value* value, CompileContext& context) const override;

    void Allocate(CompileContext& context) override;
    void Initialize(CompileContext& context) override;
};

class MemoryVariable : public Variable
{
    llvm::Value* ptr;
public:
    MemoryVariable(std::string name, LangType* type);

    llvm::Value* GetValue(CompileContext& context) const override;
    void Assign(llvm::Value* value, CompileContext& context) const override;

    void Allocate(CompileContext& context) override;
    void Initialize(CompileContext& context) override;
};

