#include "AST/Operator.h"

#include <functional>

#include "AST/Indexing.h"
#include "CompileContext.h"
#include "Operator.h"

using LangTypePtr = UnknownPtr<LangType>;

RULE(Operator)
{
    Autoname(builder);
    builder.Add("==");
    builder.Add("!=");
    builder.Add(">=");
    builder.Add("<=");
    builder.Add("+");
    builder.Add("-");
    builder.Add("*");
    builder.Add("/");
    builder.Add("<");
    builder.Add(">");
    builder.Add("%");

    builder.SetNodeType<OperatorNode>();
}

RULE(UnaryPreOperator)
{
    Autoname(builder);
    builder.Add("*");
    builder.Add("&");

    builder.SetNodeType<UnaryPreOperatorNode>();
}

RULE(UnaryPostOperator)
{
    Autoname(builder);
    builder.Add(IndexOf);

    builder.SetNodeType<UnaryPostOperatorNode>();
}

UnknownPtr<RValue> OperatorNode::Create(UnknownPtr<RValue> lhs, UnknownPtr<RValue> rhs, CompileContext& context)
{
    LangType* ltype = lhs->GetType();
    LangType* rtype = rhs->GetType();

    BinaryOperatorImpl* impl = FindImplDir(ltype, rtype);
    if (impl == nullptr)
    {
        impl = FindImplDir(rtype, ltype);

        if (impl == nullptr)
        {
            Error("cannot apply operator '", GetValue(), "' to values of type '",
                    ltype->GetName(), "' and '", rtype->GetName(), "'.");
            return nullptr;
        }
    }

    return impl->Create(lhs, rhs, context);
}

BinaryOperatorImpl* OperatorNode::FindImplDir(LangType* lhs, LangType* rhs)
{
    std::string op = GetValue();
    auto iter = lhs->members.binaryOperators.find(op);
    if (iter == lhs->members.binaryOperators.end())
        return nullptr;

    auto& mapping = iter->second;
    auto iter2 = mapping.find(rhs);
    if (iter2 == mapping.end())
        return nullptr;

    return iter2->second;
}


static std::map<std::string, int> _binary_precedence
{
    { "==", 20 },
    { "!=", 20 },
    { ">=", 20 },
    { "<=", 20 },
    { "+" , 40 },
    { "-" , 40 },
    { "*" , 60 },
    { "/" , 60 },
    { "<" , 20 },
    { ">" , 20 },
    { "%" , 30 }
};

static std::map<std::string, bool> _binary_group_right
{
    { "==", false },
    { "!=", false },
    { ">=", false },
    { "<=", false },
    { "+" , false },
    { "-" , false },
    { "*" , false },
    { "/" , false },
    { "<" , false },
    { ">" , false },
    { "%" , false }
};

int OperatorNode::GetPrecedence() const
{
    auto op = GetValue();
    auto it = _binary_precedence.find(op);
    if (it != _binary_precedence.end())
        return it->second;

    Assert(false, "unknown operator '", op, "'");
    return 100;
}

bool OperatorNode::IsGroupRight() const
{
    auto op = GetValue();
    auto it = _binary_group_right.find(op);
    if (it != _binary_group_right.end())
        return it->second;

    Assert(false, "unknown operator '", op, "'");
    return false;
}


//****  UNARY PRE  ****//

UnknownPtr<RValue> UnaryPreOperatorNode::Create(UnknownPtr<RValue> rhs, CompileContext& context)
{
    LangType* rtype = rhs->GetType();

    UnaryOperatorImpl* impl = FindOp(rtype);
    if (impl == nullptr)
    {
        Error("Cannot use operator '", GetValue(), "' on value of type '", rtype->GetName(), "'.");
        return nullptr;
    }

    return impl->Create(rhs, context);
}

UnaryOperatorImpl* UnaryPreOperatorNode::FindOp(LangType* type)
{
    std::string op = GetValue();
    if (op == "&") // '&' can't be overloaded, so it has somewhat special treatment
    {
        return &(AddressOfOperator::Instance());
    }

    auto& opMap = type->members.unaryOperators;
    auto iter = opMap.find(op);
    if (iter == opMap.end())
        return nullptr;

    return iter->second;
}

/// Unary Post ///

UnknownPtr<RValue> UnaryPostOperatorNode::Create(UnknownPtr<RValue> lhs, CompileContext& context)
{
    auto op = SafeGet<UnaryPostOperatorBase>(0);
    return op->Create(lhs, context);
}





