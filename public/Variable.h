#pragma once

#include "LangType.h"
#include "LangValue.h"
#include "CompileContext.h"

class Variable : public LValue
{
    std::string name;
    LangType* type;
    llvm::Value* value;
public:
    Variable(std::string name, LangType* type);

    void SetValue(llvm::Value* value) { this->value = value; }

    llvm::Value* GetValue() const override { return value; }
    LangType* GetType() const override { return type; }
    void Assign(const RValue& newValue, CompileContext& context) const override;
};


