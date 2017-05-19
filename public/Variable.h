#pragma once

#include "LangType.h"
#include "LangValue.h"

class CompileContext;

class Variable : public LValue
{
    std::string name;
    LangType* type;
    llvm::Value* value;
public:
    Variable(std::string name, LangType* type);
    virtual ~Variable() {}

    void SetValue(llvm::Value* value);

    llvm::Value* GetValue() const override { return value; }
    LangType* GetType() const override { return type; }
    void Assign(const RValue& newValue, CompileContext& context) const override;
};


