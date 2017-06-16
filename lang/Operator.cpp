#include "Operator.h"

#include "Errors.h"
#include "CompileContext.h"

UnknownPtr<RValue> ArrayIndexOperator::Create(UnknownPtr<RValue> lhs, std::vector<UnknownPtr<RValue>>& args, CompileContext& context)
{
    if (args.size() != 1)
    {
        // TODO: make this print line info (this version of Error won't include it)
        Error("Wrong number of indices to indexing operator, expected 1 but recieved", args.size());
        return nullptr;
    }

    LangType* arrayType = lhs->GetType();
    if (!arrayType->IsArray())
    {
        Error("The type '", arrayType->GetName() ,"' cannot be indexed. Can only index into an array.");
        return nullptr;
    }

    ArrayType* trueArrayType = static_cast<ArrayType*>(arrayType);

    auto indexVal = args[0];
    LangType* indexType = indexVal->GetType();
    LangType* intType = context.builtins->types.Get("int");
    if (!intType->IsAssignableFrom(indexType))
    {
        Error("Cannot use type '", indexType->GetName(), "' to index into an array, expecting index of type 'int'.");
        return nullptr;
    }

    auto arrayIR = lhs->GetValue(context);
    auto indexIR = indexVal->GetValue(context);

    std::vector<unsigned int> indices { 1 };
    llvm::Value* ptrVal = context.builder.CreateExtractValue(arrayIR, indices, "array_ptr");

    std::vector<llvm::Value*> indices2 { indexIR };
    llvm::Value* finalVal = context.builder.CreateGEP(ptrVal, indices2, "array_indexed");

    Ptr<PointerLValue> result = std::make_shared<PointerLValue>(finalVal, trueArrayType->GetElementType());
    return PtrCast<RValue>(result);
}


