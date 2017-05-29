#include "AST/Operator.h"

#include <functional>

using LangTypePtr = UnknownPtr<LangType>;

RULE(Operator)
{
    Autoname(builder);
    builder.Add("==");
    builder.Add("!=");
    builder.Add("+");
    builder.Add("-");
    builder.Add("*");
    builder.Add("/");
    builder.Add("<");
    builder.Add(">");

    builder.SetNodeType<OperatorNode>();
}

RULE(UnaryPreOperator)
{
    Autoname(builder);
    builder.Add("*");
    builder.Add("&");

    builder.SetNodeType<UnaryPreOperatorNode>();
}

class OperatorImpl
{
public:
    std::function<LangType*(LangType*, LangType*, CompileContext&)> type;
    std::function<llvm::Value*(llvm::Value*, llvm::Value*, CompileContext&)> value;
};

static LangType* IntOnly(LangType* a, LangType* b, CompileContext& context)
{
    LangType* intType = context.symbolTable->types.Get("int");
    if (a != intType || b != intType)
        return nullptr;
    return intType;
}

static LangType* IntOrBoolCompare(LangType* a, LangType* b, CompileContext& context)
{
    LangType* intType = context.symbolTable->types.Get("int");
    LangType* boolType = context.symbolTable->types.Get("bool");
    if (a == intType && b == intType)
        return boolType;

    if (a == boolType && b == boolType)
        return boolType;

    return nullptr;
}

static LangType* IntCompare(LangType* a, LangType* b, CompileContext& context)
{
    LangType* intType = context.symbolTable->types.Get("int");
    LangType* boolType = context.symbolTable->types.Get("bool");
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

//****  UNARY PRE  ****//

class UnaryOperatorImpl
{
public:
    //std::function<LangTypePtr(LangTypePtr, CompileContext&)> type;
    std::function<UnknownPtr<RValue>(UnknownPtr<RValue>, CompileContext&)> value;
};

//static LangTypePtr PointerDerefType(LangTypePtr type, CompileContext& context)
//{
//    if (!type->IsPointer())
//        return nullptr;
//
//    auto ptrType = type.Cast<PointerType>();
//    return ptrType->GetSubType();
//}
//
//static LangTypePtr AddrOfType(LangTypePtr type, CompileContext& context)
//{
//    return type->GetPointerTo();
//}

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






