#include "Builtins.h"

#include "LangType.h"
#include "Function.h"
#include "TypeConverter.h"

using HiddenTable = std::map<std::string, llvm::Function*>;

void DeclarePrintf(HiddenTable& table)
{
    llvm::Type* intType = Spark::TypeConverter::Get<int>();
    llvm::Type* strType = Spark::TypeConverter::Get<char>()->getPointerTo();

    auto& manager = Spark::LLVMManager::Instance();
    auto funcSig = manager.GetFuncSignatureVarargs(intType, { strType });
    llvm::Function* func = manager.DeclareFunction("printf", funcSig);
    table["printf"] = func;
}

void DeclarePrint(SymbolTable& symbolTable, HiddenTable& hiddenTable)
{
    LangType* voidType = symbolTable.types.Get("void");
    LangType* strType = symbolTable.types.Get("string");

    std::vector<LangType*> args { strType };
    std::vector<std::string> paramNames { "value" };
    Function* langfunc = symbolTable.functions.Create("print", "print", voidType, args, paramNames);

    auto& manager = Spark::LLVMManager::Instance();
    std::vector<llvm::Type*> argsIR;
    langfunc->GetIRTypes(argsIR);
    auto funcSig = manager.GetFuncSignature(langfunc->ReturnType()->GetIR(), argsIR);
    llvm::Function* func = manager.DeclareFunction("print", funcSig, llvm::GlobalValue::LinkOnceODRLinkage);
    langfunc->SetIR(func);

    auto startBlock = manager.Implement(func);
    llvm::IRBuilder<> builder(startBlock);

    llvm::Value* param1 = (func->arg_begin());
    llvm::Value* i8ptr = builder.CreateExtractValue(param1, { 1 }, "str_ptr");
    builder.CreateCall(hiddenTable.at("printf"), { i8ptr }, "result");
    builder.CreateRetVoid();
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

    llvm::Type* intType = Spark::TypeConverter::Get<int>();
    llvm::Type* ptrType = Spark::TypeConverter::Get<char>();
    ptrType = ptrType->getPointerTo();

    llvm::Type* types[2] = { intType, ptrType };
    llvm::Type* strType = llvm::StructType::create(types, "string_t");

    lt->SetIR(strType);

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
    HiddenTable hiddenTable;

    DeclarePrintf(hiddenTable);
    DeclarePrintc(symbolTable);
    DeclareMalloc(symbolTable);

    DeclarePrint(symbolTable, hiddenTable);
}


