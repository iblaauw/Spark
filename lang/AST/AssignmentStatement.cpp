#include "AST/AssignmentStatement.h"

#include "AST/Expression.h"
#include "AST/Common.h"

RULE(AssignmentStatement)
{
    Autoname(builder);
    builder.Add(ExpressionTree, OptionalWhitespace, '=', OptionalWhitespace, ExpressionTree, ';');

    builder.Ignore(1);
    builder.Ignore(3);
    builder.Ignore(5);

    builder.SetNodeType<AssignmentStatementNode>();
}

void AssignmentStatementNode::Generate(CompileContext& context)
{
    auto lhs = customChildren[0]->Evaluate(context);
    auto rhs = customChildren[1]->Evaluate(context);

    if (lhs == nullptr || rhs == nullptr)
        return;

    if (!lhs->IsLValue())
    {
        Error("Can only assign to a variable or a valid l-value");
        return;
    }

    LValue* lval = static_cast<LValue*>(lhs.Get());

    auto ltype = lval->GetType();
    auto rtype = rhs->GetType();

    if (!ltype->IsAssignableFrom(rtype))
    {
        Error("cannot assign a value of type '", rtype->GetName(), "' to l-value of type '", ltype->GetName(), "'");
        return;
    }

    llvm::Value* rval = rhs->GetValue(context);

    // TODO: add implicit converstions

    lval->Assign(rval, context);
}

