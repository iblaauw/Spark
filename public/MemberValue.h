#pragma once

#include "UnknownPtr.h"
#include "PtrUtils.h"
#include "OperatorImpl.h"
#include "LangValue.h"

class MemberValue
{
public:
    virtual std::string GetName() const = 0;
    virtual LangType* GetType() const = 0;
    virtual UnknownPtr<RValue> GetMember(UnknownPtr<RValue> thisPtr, CompileContext& context) = 0;
};

class DataMemberValue : public MemberValue
{
private:
    unsigned int index;
    LangType* type;
    std::string name;
public:
    DataMemberValue(std::string name, unsigned int memberindex, LangType* memberType);
    std::string GetName() const override { return name; }
    LangType* GetType() const override { return type; }
    UnknownPtr<RValue> GetMember(UnknownPtr<RValue> thisPtr, CompileContext& context) override;
};

