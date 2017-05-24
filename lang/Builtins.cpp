#include "Builtins.h"

#include "LangType.h"
#include "Function.h"
#include "TypeConverter.h"

void DeclarePrintf(SymbolTable& symbolTable)
{
    LangType* intType = symbolTable.types.Get("int");
    LangType* stringType = symbolTable.types.Get("string");

    std::vector<LangType*> args { stringType };
    std::vector<std::string> paramNames { "value" };
    Function* langfunc = symbolTable.functions.Create("print", "printf", intType, args, paramNames);

    langfunc->SetIRDefault();
}

void DeclarePrintc(SymbolTable& symbolTable)
{
    LangType* intType = symbolTable.types.Get("int");

    std::vector<LangType*> args { intType };
    std::vector<std::string> paramNames { "value" };
    Function* langfunc = symbolTable.functions.Create("printc", "putchar", intType, args, paramNames);

    langfunc->SetIRDefault();
}

void DeclareMalloc(SymbolTable& symbolTable)
{
    LangType* strType = symbolTable.types.Get("string");
    LangType* intType = symbolTable.types.Get("int");

    llvm::Type* i8type = Spark::TypeConverter::Get<char>();
    llvm::Type* i8ptr = i8type->getPointerTo();

    auto& manager = Spark::LLVMManager::Instance();
    std::vector<LangType*> args { intType };
    std::vector<std::string> paramNames { "size" };
    Function* func = symbolTable.functions.Create("malloc", "malloc", strType, args, paramNames);

    std::vector<llvm::Type*> argsIR;
    func->GetIRTypes(argsIR);
    auto sig = manager.GetFuncSignature(i8ptr, argsIR);
    llvm::Function* funcIR = manager.DeclareFunction(func->GetName(), sig);
    func->SetIR(funcIR);
}

template <class T>
LangType* _CreateType(std::string name)
{
    llvm::Type* type = Spark::TypeConverter::Get<T>();
    BasicType* lt = new BasicType(name);
    lt->SetIR(type);
    return lt;
}

LangType* _CreateStringType()
{
    BasicType* lt = new BasicType("string");

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

void AddBuiltinFunctions(SymbolTable& symbolTable)
{
    DeclarePrintf(symbolTable);
    DeclarePrintc(symbolTable);
    DeclareMalloc(symbolTable);
}


