#pragma once

#include "LangType.h"
#include "LangValue.h"

class Variable : public LValue
{
    std::string name;
    LangType* type;
    llvm::Value* value;
public:
    Variable(std::string name, LangType* type);

    std::string GetName() const { return name; }

    void SetValue(llvm::Value* value);

    llvm::Value* GetValue(CompileContext& context) const override { return value; }
    LangType* GetType() const override { return type; }
    void Assign(const RValue& newValue, CompileContext& context) const override;
};

class MemoryVariable : public LValue
{
    std::string name;
    LangType* type;
    llvm::Value* ptr;
public:
    MemoryVariable(std::string name, LangType* type);

    std::string GetName() const { return name; }

    void SetValue(llvm::Value* value);

    llvm::Value* GetValue(CompileContext& context) const override;
    LangType* GetType() const override { return type; }
    void Assign(const RValue& newValue, CompileContext& context) const override;

};

