#include "AST/Indexing.h"

#include "TypeConverter.h"
#include "AST/Common.h"
#include "AST/Expression.h"
#include "AST/VariableExpression.h"

RULE(IndexOf)
{
    Autoname(builder);
    builder.Add(VariableExpression, OptionalWhitespace, '[', OptionalWhitespace, ExpressionTree, OptionalWhitespace, ']');
    builder.Ignore(1);
    builder.Ignore(3);
    builder.Ignore(5);

    builder.SetNodeType<IndexOfNode>();
}

UnknownPtr<RValue> IndexOfNode::Evaluate(CompileContext& context)
{
    Assert(customChildren.size() >= 2, "Invalid 'Index Of' Expression structure");

    auto arrayVal = customChildren[0]->Evaluate(context);
    if (arrayVal == nullptr)
        return nullptr;

    LangType* arrayType = arrayVal->GetType();
    if (!arrayType->IsArray())
    {
        Error("The type '", arrayType->GetName() ,"' cannot be indexed. Can only index into an array.");
        return nullptr;
    }

    ArrayType* trueArrayType = static_cast<ArrayType*>(arrayType);

    auto indexVal = customChildren[1]->Evaluate(context);
    if (indexVal == nullptr)
        return nullptr;

    LangType* indexType = indexVal->GetType();
    LangType* intType = context.builtins->types.Get("int");
    if (!intType->IsAssignableFrom(indexType))
    {
        Error("Cannot use type '", indexType->GetName(), "' to index into an array, expecting index of type 'int'.");
        return nullptr;
    }

    auto arrayIR = arrayVal->GetValue(context);
    auto indexIR = indexVal->GetValue(context);

    std::vector<unsigned int> indices { 1 };
    llvm::Value* ptrVal = context.builder.CreateExtractValue(arrayIR, indices, "array_ptr");

    std::vector<llvm::Value*> indices2 { indexIR };
    llvm::Value* finalVal = context.builder.CreateGEP(ptrVal, indices2, "array_indexed");

    Ptr<PointerLValue> result = std::make_shared<PointerLValue>(finalVal, trueArrayType->GetElementType());
    return PtrCast<RValue>(result);
}

