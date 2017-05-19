#pragma once

#include "llvm/IR/Type.h"

class CompileContext;

class LangType
{
private:
    std::string name;
    llvm::Type* type;

    // TODO: add a map of members
public:
    LangType(std::string name);

    std::string GetName() const { return name; }

    llvm::Type* GetIR() const { return type; }
    void SetIR(llvm::Type* type) { this->type = type; }

    bool IsAssignableFrom(LangType& otherType) const;
    void InsertConversion(LangType& fromType, CompileContext& context) const;

    // TODO: add inheritance and const-ness
};


