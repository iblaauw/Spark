#include "Builtins.h"

#include "LangType.h"
#include "Function.h"
#include "TypeConverter.h"

llvm::Function* DeclarePrintf(SymbolTable& symbolTable)
{
    LangType* intType = symbolTable.types.Get("int");
    LangType* stringType = symbolTable.types.Get("string");

    auto& manager = Spark::LLVMManager::Instance();

    std::vector<LangType*> args { stringType };
    std::vector<std::string> paramNames { "value" };
    Function* langfunc = symbolTable.functions.Create("print", "printf", intType, args, paramNames);

    std::vector<llvm::Type*> argsIR;
    langfunc->GetIRTypes(argsIR);

    auto sig = manager.GetFuncSignatureVarargs(intType->GetIR(), argsIR);
    llvm::Function* func = manager.DeclareFunction(langfunc->GetName(), sig);

    langfunc->SetIR(func);

    return func;
}

llvm::Function* DeclarePrintc(SymbolTable& symbolTable)
{
    LangType* intType = symbolTable.types.Get("int");

    auto& manager = Spark::LLVMManager::Instance();
    std::vector<LangType*> args { intType };
    std::vector<std::string> paramNames { "value" };
    Function* langfunc = symbolTable.functions.Create("printc", "putchar", intType, args, paramNames);

    std::vector<llvm::Type*> argsIR;
    langfunc->GetIRTypes(argsIR);

    auto sig = manager.GetFuncSignature(intType->GetIR(), argsIR);
    llvm::Function* func = manager.DeclareFunction(langfunc->GetName(), sig);

    langfunc->SetIR(func);

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
    symbolTable.types.Add(type->GetName(), type);

    type = _CreateType<int>("int");
    symbolTable.types.Add(type->GetName(), type);

    type = _CreateType<char>("char");
    symbolTable.types.Add(type->GetName(), type);

    type = _CreateType<bool>("bool");
    symbolTable.types.Add(type->GetName(), type);

    type = _CreateStringType();
    symbolTable.types.Add(type->GetName(), type);
}


