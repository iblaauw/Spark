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

#include "llvm/Support/raw_ostream.h" // for llvm::errs()

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

    std::cerr << "BLAHBLAH" << std::endl;

    auto arrayIR = arrayVal->GetValue(context);
    auto indexIR = indexVal->GetValue(context);

    std::cerr << "A" << std::endl;

    llvm::errs() << *arrayIR << "\n";
    llvm::errs() << *indexIR << "\n";

    std::vector<llvm::Value*> indices { indexIR };
    llvm::Value* finalVal = context.builder.CreateGEP(arrayIR, indices);

    std::cerr << "B" << std::endl;

    Ptr<PointerLValue> result = std::make_shared<PointerLValue>(finalVal, trueArrayType->GetElementType());
    std::cerr << "BLAHBLAH" << std::endl;
    return PtrCast<RValue>(result);
}

