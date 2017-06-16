#include "AST/Operator.h"

#include <functional>

#include "AST/Indexing.h"
#include "CompileContext.h"

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

class UnaryOperatorNodeImpl
{
public:
    std::function<UnknownPtr<RValue>(UnknownPtr<RValue>, CompileContext&)> value;
};

static UnknownPtr<RValue> PointerDeref(UnknownPtr<RValue> rval, CompileContext& context)
{
    LangType* type = rval->GetType();
    if (!type->IsPointer())
    {
        Error("Cannot dereference a value that isn't a pointer.\n Expected pointer but received type '", type->GetName(), "'");
        return nullptr;
    }

    PointerType* ptrType = static_cast<PointerType*>(type);
    LangType* resultType = ptrType->GetSubType();

    llvm::Value* result = rval->GetValue(context);
    if (result == nullptr)
        return nullptr;

    Ptr<PointerLValue> lval_result = std::make_shared<PointerLValue>(result, resultType);
    return PtrCast<RValue>(lval_result);
}

static UnknownPtr<RValue> AddrOfValue(UnknownPtr<RValue> rval, CompileContext& context)
{
    if (!rval->IsLValue())
    {
        Error("Can only find the address of a variable or valid l-value");
        return nullptr;
    }

    UnknownPtr<LValue> lval = rval.Cast<LValue>();
    llvm::Value* result = lval->GetAddress(context);
    LangType* resultType = lval->GetType()->GetPointerTo();
    auto resultValue = std::make_shared<GeneralRValue>(result, resultType);
    return PtrCast<RValue>(resultValue);
}

void UnaryPreOperatorNode::Process()
{
    StringValueNode::Process();

    std::string val = GetValue();
    impl = std::make_shared<UnaryOperatorNodeImpl>();

    if (val == "*")
    {
        impl->value = PointerDeref;
    }
    else if (val == "&")
    {
        impl->value = AddrOfValue;
    }
    else
    {
        Assert(false, "Unknown unary operator '", val, "'");
    }
}


UnknownPtr<RValue> UnaryPreOperatorNode::Create(UnknownPtr<RValue> rhs, CompileContext& context)
{
    Assert(impl != nullptr, "null impl");
    return impl->value(rhs, context);
}

/// Unary Post ///

UnknownPtr<RValue> UnaryPostOperatorNode::Create(UnknownPtr<RValue> lhs, CompileContext& context)
{
    auto op = SafeGet<UnaryPostOperatorBase>(0);
    return op->Create(lhs, context);
}





