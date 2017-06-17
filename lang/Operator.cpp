#include "Operator.h"

#include <functional>

#include "Errors.h"
#include "CompileContext.h"

//// ArrayIndexOperator ////

UnknownPtr<RValue> ArrayIndexOperator::Create(UnknownPtr<RValue> lhs, std::vector<UnknownPtr<RValue>>& args, CompileContext& context)
{
    if (args.size() != 1)
    {
        // TODO: make this print line info (this version of Error won't include it)
        Error("Wrong number of indices to indexing operator, expected 1 but recieved", args.size());
        return nullptr;
    }

    LangType* arrayType = lhs->GetType();
    if (!arrayType->IsArray())
    {
        Error("The type '", arrayType->GetName() ,"' cannot be indexed. Can only index into an array.");
        return nullptr;
    }

    ArrayType* trueArrayType = static_cast<ArrayType*>(arrayType);

    auto indexVal = args[0];
    LangType* indexType = indexVal->GetType();
    LangType* intType = context.builtins->types.Get("int");
    if (!intType->IsAssignableFrom(indexType))
    {
        Error("Cannot use type '", indexType->GetName(), "' to index into an array, expecting index of type 'int'.");
        return nullptr;
    }

    auto arrayIR = lhs->GetValue(context);
    auto indexIR = indexVal->GetValue(context);

    std::vector<unsigned int> indices { 1 };
    llvm::Value* ptrVal = context.builder.CreateExtractValue(arrayIR, indices, "array_ptr");

    std::vector<llvm::Value*> indices2 { indexIR };
    llvm::Value* finalVal = context.builder.CreateGEP(ptrVal, indices2, "array_indexed");

    Ptr<PointerLValue> result = std::make_shared<PointerLValue>(finalVal, trueArrayType->GetElementType());
    return PtrCast<RValue>(result);
}

//// DereferenceOperator ////

RValuePtr DereferenceOperator::Create(RValuePtr rhs, CompileContext& context)
{
    LangType* type = rhs->GetType();
    if (!type->IsPointer())
    {
        Error("Cannot dereference a value that isn't a pointer.\n Expected pointer but received type '", type->GetName(), "'");
        return nullptr;
    }

    PointerType* ptrType = static_cast<PointerType*>(type);
    LangType* resultType = ptrType->GetSubType();

    llvm::Value* result = rhs->GetValue(context);
    if (result == nullptr)
        return nullptr;

    Ptr<PointerLValue> lval_result = std::make_shared<PointerLValue>(result, resultType);
    return PtrCast<RValue>(lval_result);
}

//// AddressOfOperator ////

RValuePtr AddressOfOperator::Create(RValuePtr rhs, CompileContext& context)
{
    if (!rhs->IsLValue())
    {
        Error("Can only find the address of a variable or valid l-value");
        return nullptr;
    }

    UnknownPtr<LValue> lval = rhs.Cast<LValue>();
    llvm::Value* result = lval->GetAddress(context);
    LangType* resultType = lval->GetType()->GetPointerTo();
    auto resultValue = std::make_shared<GeneralRValue>(result, resultType);
    return PtrCast<RValue>(resultValue);
}

//// FuncCallOperator ////

RValuePtr FuncCallOperator::Create(RValuePtr lhs, std::vector<RValuePtr>& argVals, CompileContext& context)
{
    LangType* ltype = lhs->GetType();
    Assert(ltype->IsFunction(), "Expected function but recieved a non-function");

    FunctionType* funcType = static_cast<FunctionType*>(ltype);
    if (!IsCompatible(funcType, argVals))
        return nullptr;

    auto funcDef = lhs->GetValue(context);
    std::vector<llvm::Value*> args;
    auto converter = [&context](UnknownPtr<RValue> rv) { return rv->GetValue(context); };
    ::Map(converter, argVals, args);

    llvm::Value* value = context.builder.CreateCall(funcDef, args);

    LangType* retType = funcType->ReturnType();
    auto ptrVal = std::make_shared<GeneralRValue>(value, retType);
    return PtrCast<RValue>(ptrVal);
}

bool FuncCallOperator::IsCompatible(FunctionType* funcType, std::vector<RValuePtr>& args)
{
    const auto& paramTypes = funcType->ParameterTypes();
    if (args.size() != paramTypes.size())
    {
        Error("wrong number of arguments to call a function '", funcType->GetName(), "'\n",
                "Expected ", paramTypes.size(), " arguments but recieved ", args.size());
        return false;
    }

    auto converter = [](UnknownPtr<RValue> rv) { return rv->GetType(); };
    std::vector<LangType*> argTypes;
    ::Map(converter, args, argTypes);

    for (unsigned int i = 0; i < paramTypes.size(); i++)
    {
        LangType* a = argTypes[i];
        LangType* p = paramTypes[i];

        if (!p->IsAssignableFrom(a))
        {
            Error("invalid argument ", i, " when calling a function '", funcType->GetName(), "'\n",
                    "Cannot convert from type '", a->GetName(), "' to '", p->GetName(), "'");
            return false;
        }
    }

    return true;
}

//// BasicBinaryOperator ////

using ValueFactory = std::function<llvm::Value*(llvm::Value*, llvm::Value*, CompileContext&)>;

class BasicBinaryOperator : public BinaryOperatorImpl
{
private:
    std::string retType;
    ValueFactory factory;
public:
    BasicBinaryOperator(ValueFactory factory, std::string retType)
        : retType(retType), factory(factory) {}

    RValuePtr Create(RValuePtr lhs, RValuePtr rhs, CompileContext& context) override
    {
        LangType* intType = context.builtins->types.Get(retType);
        auto valL = lhs->GetValue(context);
        auto valR = rhs->GetValue(context);

        llvm::Value* resultVal = factory(valL, valR, context);

        Ptr<GeneralRValue> result = std::make_shared<GeneralRValue>(resultVal, intType);
        return PtrCast<RValue>(result);
    }
};

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

void AddIntOperators(LangType* intType)
{
    auto& ops = intType->members.binaryOperators;
    ops["+"][intType] = new BasicBinaryOperator(PlusVal, "int");
    ops["-"][intType] = new BasicBinaryOperator(SubtractVal, "int");
    ops["*"][intType] = new BasicBinaryOperator(MultiplyVal, "int");
    ops["/"][intType] = new BasicBinaryOperator(DivideVal, "int");
    ops["=="][intType] = new BasicBinaryOperator(EqualsVal, "bool");
    ops["!="][intType] = new BasicBinaryOperator(NotEqualsVal, "bool");
    ops[">"][intType] = new BasicBinaryOperator(GreaterThanVal, "bool");
    ops["<"][intType] = new BasicBinaryOperator(LessThanVal, "bool");
    ops[">="][intType] = new BasicBinaryOperator(GreaterEqualVal, "bool");
    ops["<="][intType] = new BasicBinaryOperator(LessEqualVal, "bool");
    ops["%"][intType] = new BasicBinaryOperator(ModuloVal, "int");
}

void AddBoolOperators(LangType* boolType)
{
    auto& ops = boolType->members.binaryOperators;
    ops["=="][boolType] = new BasicBinaryOperator(EqualsVal, "bool");
    ops["!="][boolType] = new BasicBinaryOperator(NotEqualsVal, "bool");
}

