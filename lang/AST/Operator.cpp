#include "AST/Operator.h"

#include <functional>

#include "AST/Indexing.h"

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

class OperatorImpl
{
public:
    std::function<LangType*(LangType*, LangType*, CompileContext&)> type;
    std::function<llvm::Value*(llvm::Value*, llvm::Value*, CompileContext&)> value;
};

static LangType* IntOnly(LangType* a, LangType* b, CompileContext& context)
{
    LangType* intType = context.builtins->types.Get("int");
    if (a != intType || b != intType)
        return nullptr;
    return intType;
}

static LangType* IntOrBoolCompare(LangType* a, LangType* b, CompileContext& context)
{
    LangType* intType = context.builtins->types.Get("int");
    LangType* boolType = context.builtins->types.Get("bool");
    if (a == intType && b == intType)
        return boolType;

    if (a == boolType && b == boolType)
        return boolType;

    return nullptr;
}

static LangType* IntCompare(LangType* a, LangType* b, CompileContext& context)
{
    LangType* intType = context.builtins->types.Get("int");
    LangType* boolType = context.builtins->types.Get("bool");
    if (a == intType && b == intType)
        return boolType;

    return nullptr;
}

static llvm::Value* PlusVal(llvm::Value* a, llvm::Value* b, CompileContext& context)
{
    return context.builder.CreateAdd(a, b, "int_add");
}

static llvm::Value* SubtractVal(llvm::Value* a, llvm::Value* b, CompileContext& context)
{
    return context.builder.CreateSub(a, b, "int_sub");
}

static llvm::Value* MultiplyVal(llvm::Value* a, llvm::Value* b, CompileContext& context)
{
    return context.builder.CreateMul(a,b, "int_mul");
}

static llvm::Value* DivideVal(llvm::Value* a, llvm::Value* b, CompileContext& context)
{
    return context.builder.CreateSDiv(a,b, "int_div");
}

static llvm::Value* EqualsVal(llvm::Value* a, llvm::Value* b, CompileContext& context)
{
    return context.builder.CreateICmpEQ(a, b, "equals");
}

static llvm::Value* NotEqualsVal(llvm::Value* a, llvm::Value* b, CompileContext& context)
{
    return context.builder.CreateICmpNE(a, b, "not_equals");
}

static llvm::Value* GreaterThanVal(llvm::Value* a, llvm::Value* b, CompileContext& context)
{
    return context.builder.CreateICmpSGT(a, b, "greater");
}

static llvm::Value* LessThanVal(llvm::Value* a, llvm::Value* b, CompileContext& context)
{
    return context.builder.CreateICmpSLT(a, b, "less");
}

static llvm::Value* GreaterEqualVal(llvm::Value* a, llvm::Value* b, CompileContext& context)
{
    return context.builder.CreateICmpSGE(a, b, "greater_equal");
}

static llvm::Value* LessEqualVal(llvm::Value* a, llvm::Value* b, CompileContext& context)
{
    return context.builder.CreateICmpSLE(a, b, "less_equal");
}

static llvm::Value* ModuloVal(llvm::Value* a, llvm::Value* b, CompileContext& context)
{
    return context.builder.CreateSRem(a, b, "mod");
}

void OperatorNode::Process()
{
    StringValueNode::Process();

    impl = std::make_shared<OperatorImpl>();

    std::string op = GetValue();
    if (op == "+")
    {
        impl->type = IntOnly;
        impl->value = PlusVal;
    }
    else if (op == "-")
    {
        impl->type = IntOnly;
        impl->value = SubtractVal;
    }
    else if (op == "*")
    {
        impl->type = IntOnly;
        impl->value = MultiplyVal;
    }
    else if (op == "/")
    {
        impl->type = IntOnly;
        impl->value = DivideVal;
    }
    else if (op == "==")
    {
        impl->type = IntOrBoolCompare;
        impl->value = EqualsVal;
    }
    else if (op == "!=")
    {
        impl->type = IntOrBoolCompare;
        impl->value = NotEqualsVal;
    }
    else if (op == "<")
    {
        impl->type = IntCompare;
        impl->value = LessThanVal;
    }
    else if (op == ">")
    {
        impl->type = IntCompare;
        impl->value = GreaterThanVal;
    }
    else if (op == "<=")
    {
        impl->type = IntCompare;
        impl->value = LessEqualVal;
    }
    else if (op == ">=")
    {
        impl->type = IntCompare;
        impl->value = GreaterEqualVal;
    }
    else if (op == "%")
    {
        impl->type = IntOnly;
        impl->value = ModuloVal;
    }
    else
    {
        Assert(false, "unknown operator '", op, "'");
    }
}

LangType* OperatorNode::GetResultType(LangType* lhs, LangType* rhs, CompileContext& context)
{
    return impl->type(lhs, rhs, context);
}

llvm::Value* OperatorNode::Create(llvm::Value* lhs, llvm::Value* rhs, CompileContext& context)
{
    return impl->value(lhs, rhs, context);
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

class UnaryOperatorImpl
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
    impl = std::make_shared<UnaryOperatorImpl>();

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





