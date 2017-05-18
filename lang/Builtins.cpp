#include "Builtins.h"

#include "LangType.h"
#include "Function.h"
#include "TypeConverter.h"

llvm::Function* DeclarePrintf(SymbolTable& symbolTable)
{
    Ptr<LangType> intType = symbolTable.types.Get("int");
    Ptr<LangType> stringType = symbolTable.types.Get("string");

    auto& manager = Spark::LLVMManager::Instance();

    std::vector<Ptr<LangType>> args { stringType };
    std::vector<std::string> paramNames { "value" };
    Ptr<Function> langfunc = std::make_shared<Function>("printf", intType, args, paramNames);

    std::vector<llvm::Type*> argsIR;
    langfunc->GetIRTypes(argsIR);

    auto sig = manager.GetFuncSignatureVarargs(intType->GetIR(), argsIR);
    llvm::Function* func = manager.DeclareFunction(langfunc->GetName(), sig);

    langfunc->SetIR(func);

    symbolTable.functions.Add("print", langfunc);
    return func;
}

template <class T>
Ptr<LangType> _CreateType(std::string name)
{
    llvm::Type* type = Spark::TypeConverter::Get<T>();
    Ptr<LangType> lt = std::make_shared<LangType>(name);
    lt->SetIR(type);
    return lt;
}

Ptr<LangType> _CreateStringType()
{
    Ptr<LangType> lt = std::make_shared<LangType>("string");

    llvm::Type* type = Spark::TypeConverter::Get<char>();
    type = type->getPointerTo();

    lt->SetIR(type);

    return lt;
}

void AddBuiltinTypes(SymbolTable& symbolTable)
{
    Ptr<LangType> type;

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


