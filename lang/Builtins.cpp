#include "Builtins.h"

#include "LangType.h"
#include "Function.h"
#include "TypeConverter.h"

llvm::Function* DeclarePrintf(SymbolTable& symbolTable)
{
    LangType* intType = symbolTable.GetType("int");
    LangType* stringType = symbolTable.GetType("string");

    auto& manager = Spark::LLVMManager::Instance();

    std::vector<LangType*> args { stringType };
    std::vector<std::string> paramNames { "value" };
    Function* langfunc = new Function("printf", intType, args, paramNames);

    std::vector<llvm::Type*> argsIR;
    langfunc->GetIRTypes(argsIR);

    auto sig = manager.GetFuncSignatureVarargs(intType->GetIR(), argsIR);
    llvm::Function* func = manager.DeclareFunction(langfunc->GetName(), sig);

    langfunc->SetIR(func);

    symbolTable.AddFunction("print", langfunc);
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

    type = _CreateType<char>("char");
    symbolTable.AddType(type->GetName(), type);

    type = _CreateType<bool>("bool");
    symbolTable.AddType(type->GetName(), type);

    type = _CreateStringType();
    symbolTable.AddType(type->GetName(), type);
}


