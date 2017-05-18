#pragma once

#include "LangType.h"
#include "LangValue.h"

class CompileContext;

class Variable : public LValue
{
    std::string name;
    Ptr<LangType> type;
    llvm::Value* value;
public:
    Variable(std::string name, Ptr<LangType> type);

    void SetValue(llvm::Value* value) { this->value = value; }

    llvm::Value* GetValue() const override { return value; }
    Ptr<LangType> GetType() const override { return type; }
    void Assign(const RValue& newValue, CompileContext& context) const override;
};


