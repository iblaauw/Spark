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
        std::cerr << "Error: Can only assign to a variable or a valid l-value" << std::endl;
        return;
    }

    LValue* lval = static_cast<LValue*>(lhs.Get());

    lval->Assign(*rhs, context);
}
