#include "Builtins.h"

#include "LangType.h"
#include "Function.h"
#include "TypeConverter.h"
#include "Operator.h"
#include "MemberValue.h"

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
    FunctionType* funcType = FunctionType::Create(voidType, args);
    Function* langfunc = symbolTable.functions.Create("print", "print", funcType, paramNames);

    auto& manager = Spark::LLVMManager::Instance();
    std::vector<llvm::Type*> argsIR;
    langfunc->GetIRTypes(argsIR);
    auto funcSig = manager.GetFuncSignature(funcType->ReturnType()->GetIR(), argsIR);
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
    FunctionType* funcType = FunctionType::Create(intType, args);
    Function* langfunc = symbolTable.functions.Create("printc", "putchar", funcType, paramNames);

    langfunc->SetIRDefault();
}

void DeclareMalloc(SymbolTable& symbolTable)
{
    LangType* strType = symbolTable.types.Get("string");
    LangType* intType = symbolTable.types.Get("int");

    std::vector<LangType*> args { intType };
    std::vector<std::string> paramNames { "size" };
    FunctionType* funcType = FunctionType::Create(strType, args);
    Function* func = symbolTable.functions.Create("malloc", "malloc", funcType, paramNames);

    func->SetIRDefault();
}

template <class T>
LangType* _CreateType(std::string name)
{
    llvm::Type* type = Spark::TypeConverter::Get<T>();
    BasicType* lt = new BasicType(name);
    lt->SetIR(type);
    return lt;
}

LangType* _CreateStringType(LangType* intLangType)
{
    BasicType* lt = new BasicType("string");

    llvm::Type* intType = Spark::TypeConverter::Get<int>();
    llvm::Type* ptrType = Spark::TypeConverter::Get<char>();
    ptrType = ptrType->getPointerTo();

    llvm::Type* types[2] = { intType, ptrType };
    llvm::Type* strType = llvm::StructType::create(types, "string_t");

    lt->SetIR(strType);

    lt->members.named["size"] = new DataMemberValue("size", 0, intLangType);

    return lt;
}

void AddBuiltinTypes(SymbolTable& symbolTable)
{
    LangType* type;

    type = _CreateType<void>("void");
    symbolTable.types.Add(type->GetName(), type);

    type = _CreateType<int>("int");
    symbolTable.types.Add(type->GetName(), type);
    AddIntOperators(type);
    LangType::builtinInt = type;

    type = _CreateType<char>("char");
    symbolTable.types.Add(type->GetName(), type);

    type = _CreateType<bool>("bool");
    symbolTable.types.Add(type->GetName(), type);
    AddBoolOperators(type);

    type = _CreateStringType(LangType::builtinInt);
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


