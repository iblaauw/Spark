#include "AST/Expression.h"
#include "AST/Common.h"
#include "AST/StringLiteral.h"
#include "AST/NumberLiteral.h"
#include "AST/FuncCall.h"
#include "AST/VariableExpression.h"
#include "AST/Operator.h"


RULE(Expression)
{
    Autoname(builder);
    builder.Add(NumberLiteral);
    builder.Add(StringLiteral);
    builder.Add(FuncCall);
    builder.Add(VariableExpression);

    builder.SetNodeType<ExpressionNode>();
}

RULE(ExpressionTree)
{
    Autoname(builder);
    builder.Add(Expression, OptionalWhitespace, Operator, OptionalWhitespace, ExpressionTree);
    builder.Add(Expression);

    builder.Ignore(1);
    builder.Ignore(3);

    builder.SetNodeType<ExpressionTreeNode>();
}

UnknownPtr<RValue> ExpressionNode::Evaluate(CompileContext& context)
{
    if (customChildren.size() == 0)
    {
        std::cerr << "Internal Error: empty expression" << std::endl;
        return nullptr;
    }

    auto child = customChildren[0];
    return child->Evaluate(context);
}

UnknownPtr<RValue> ExpressionTreeNode::Evaluate(CompileContext& context)
{
    if (customChildren.size() == 0)
    {
        std::cerr << "Internal Error: empty expression tree" << std::endl;
    }

    auto lhs = customChildren[0]->Evaluate(context);
    if (lhs == nullptr)
        return nullptr;

    if (customChildren.size() == 1)
        return lhs;

    Ptr<OperatorNode> op = SafeGet<OperatorNode>(1, "OperatorNode");
    if (op == nullptr)
    {
        std::cerr << "Internal Error: invalid operator node in expression tree." << std::endl;
        return nullptr;
    }

    if (customChildren.size() < 3)
    {
        std::cerr << "Internal Error: missing portion of expression tree." << std::endl;
        return nullptr;
    }

    auto rhs = customChildren[2]->Evaluate(context);
    if (rhs == nullptr)
        return nullptr;

    LangType* ltype = lhs->GetType();
    LangType* rtype = rhs->GetType();
    LangType* resultType = op->GetResultType(ltype, rtype, context);
    if (resultType == nullptr)
    {
        std::cerr << "Error: cannot apply operator '" << op->GetValue()
            << "' to values of type '" << ltype->GetName() << "' and '"
            << rtype->GetName() << "'." << std::endl;
        return nullptr;
    }

    llvm::Value* lval = lhs->GetValue(context);
    llvm::Value* rval = rhs->GetValue(context);
    llvm::Value* resultValue = op->Create(lval, rval, context);
    auto finalResult = std::make_shared<GeneralRValue>(resultValue, resultType);
    return PtrCast<RValue>(finalResult);
}

