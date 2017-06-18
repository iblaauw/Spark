#pragma once

#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "PtrUtils.h"
#include "SpecialTypeCache.h"
#include "UnknownPtr.h"
#include "MemberTable.h"

class CompileContext;
class LValue;
class RValue;

class LangType
{
private:
    SpecialTypeCache cache;
public:
    MemberTable members;

    LangType() : cache(this) {}
    virtual ~LangType() {}
    virtual std::string GetName() const = 0;
    virtual llvm::Type* GetIR() const = 0;

    virtual bool IsPointer() const { return false; }
    virtual bool IsArray() const { return false; }
    virtual bool IsFunction() const { return false; }

    virtual bool IsAssignableFrom(LangType* otherType) const = 0;
    virtual void InsertConversion(LangType* fromType, CompileContext& context) const = 0;

    virtual void CallConstructor(UnknownPtr<LValue> lval,
                                std::vector<UnknownPtr<RValue>>& arguments,
                                CompileContext& context) { }

    LangType* GetPointerTo() { return cache.GetPointer(); }
    LangType* GetArrayOf(int size) { return cache.GetArray(size); }

    static LangType* builtinInt; // TODO: this is really hacky
    // TODO: add inheritance and const-ness
};

class BasicType : public LangType
{
private:
    std::string name;
    llvm::Type* type;
public:
    BasicType(std::string name);

    std::string GetName() const override { return name; }

    llvm::Type* GetIR() const override { return type; }
    void SetIR(llvm::Type* type) { this->type = type; }

    bool IsAssignableFrom(LangType* otherType) const override;
    void InsertConversion(LangType* fromType, CompileContext& context) const override;
};

class PointerType : public LangType
{
private:
    LangType* subType;

    PointerType(LangType* subType);
public:
    std::string GetName() const override { return subType->GetName() + "*"; }
    llvm::Type* GetIR() const override;

    bool IsPointer() const override { return true; }

    bool IsAssignableFrom(LangType* otherType) const override;
    void InsertConversion(LangType* fromType, CompileContext& context) const override;

    LangType* GetSubType() const { return subType; }

    friend class SpecialTypeCache;
};

class ArrayType : public LangType
{
private:
    llvm::Type* type;
    LangType* subType;
    int size;

    ArrayType(llvm::Type* irType, LangType* subType, int size);
public:
    std::string GetName() const override { return subType->GetName() + "[" + std::to_string(size) + "]"; }
    llvm::Type* GetIR() const override;

    bool IsArray() const override { return true; }

    bool IsAssignableFrom(LangType* otherType) const override;
    void InsertConversion(LangType* fromType, CompileContext& context) const override;

    void CallConstructor(UnknownPtr<LValue> lval,
                        std::vector<UnknownPtr<RValue>>& arguments,
                        CompileContext& context) override;

    LangType* GetElementType() const { return subType; }
    int GetSize() const { return size; }

private:
    static llvm::Type* CreateMasterType(LangType* subType);

    friend class SpecialTypeCache;
};

class FunctionType : public LangType
{
private:
    static std::map<std::string, FunctionType*> funcCache;

    LangType* returnType;
    std::vector<LangType*> parameterTypes;
    llvm::FunctionType* irType;

    FunctionType(LangType* retType, const std::vector<LangType*>& params);
public:
    std::string GetName() const override { return GetName(returnType, parameterTypes); }
    llvm::Type* GetIR() const override { return irType; }

    bool IsFunction() const override { return true; }

    llvm::FunctionType* GetFuncIR() const { return irType; }

    bool IsAssignableFrom(LangType* otherType) const override;
    void InsertConversion(LangType* fromType, CompileContext& context) const override;

    LangType* ReturnType() const { return returnType; }
    const std::vector<LangType*>& ParameterTypes() const { return parameterTypes; }

    static FunctionType* Create(LangType* retType, const std::vector<LangType*>& params);
private:
    static std::string GetName(LangType* retType, const std::vector<LangType*>& params);
};


