#include "AST/Indexing.h"

#include "TypeConverter.h"
#include "AST/Common.h"
#include "AST/Expression.h"

RULE(IndexOf)
{
    Autoname(builder);
    builder.Add('[', OptionalWhitespace, ExpressionTree, OptionalWhitespace, ']');
    builder.Ignore(1);
    builder.Ignore(3);

    builder.SetNodeType<IndexOfNode>();
}

UnknownPtr<RValue> IndexOfNode::Create(UnknownPtr<RValue> lhs, CompileContext& context)
{
    Assert(customChildren.size() >= 1, "Invalid 'Index Of' Expression structure");

    auto indexVal = customChildren[0]->Evaluate(context);
    if (indexVal == nullptr)
        return nullptr;

    std::vector<UnknownPtr<RValue>> opArgs { indexVal };

    LangType* leftType = lhs->GetType();
    auto op = leftType->members.indexOperator;
    if (op == nullptr)
    {
        Error("The type '", leftType->GetName() ,"' cannot be indexed.");
        return nullptr;
    }

    return op->Create(lhs, opArgs, context);
}

