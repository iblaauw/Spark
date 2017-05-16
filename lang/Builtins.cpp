#include "Builtins.h"

#include "LangType.h"
#include "TypeConverter.h"

llvm::Function* DeclarePrintf(SymbolTable& symbolTable)
{

    llvm::Type* intType = Spark::TypeConverter::Get<int>();
    llvm::Type* charType = Spark::TypeConverter::Get<char>();

    auto& manager = Spark::LLVMManager::Instance();

    std::vector<llvm::Type*> args { charType->getPointerTo() };
    auto sig = manager.GetFuncSignatureVarargs(intType, args);
    llvm::Function* func = manager.DeclareFunction("printf", sig);

    symbolTable.AddFunction("print", func);
    return func;
}

template <class T>
LangType* _CreateType(std::string name)
{
    llvm::Type* type = Spark::TypeConverter::Get<T>();
    LangType* lt = new LangType(name);
    lt->SetIR(type);
    return lt;
}

LangType* _CreateStringType()
{
    LangType* lt = new LangType("string");

    llvm::Type* type = Spark::TypeConverter::Get<char>();
    type = type->getPointerTo();

    lt->SetIR(type);

    return lt;
}

void AddBuiltinTypes(SymbolTable& symbolTable)
{
    LangType* type;

    type = _CreateType<void>("void");
    symbolTable.AddType(type->GetName(), type);

    type = _CreateType<int>("int");
    symbolTable.AddType(type->GetName(), type);

    type = _CreateType<bool>("bool");
    symbolTable.AddType(type->GetName(), type);

    type = _CreateStringType();
    symbolTable.AddType(type->GetName(), type);
}


