#include "AST/StringLiteral.h"
#include "LLVMManager.h"
#include "TypeConverter.h"

CHARSET(StringLiteralChar)
{
    return c != '"' && c != '\n';
}

RULE(_StringContents)
{
    Autoname(builder);
    builder.AddString(StringLiteralChar);
}

RULE(StringLiteral)
{
    Autoname(builder);
    builder.Add('"', _StringContents, '"');
    builder.Ignore(0);
    builder.Ignore(2);

    builder.SetNodeType<StringLiteralNode>();
}

void StringLiteralNode::Process()
{
    // TODO: make more robust
    NodePtr child = children[0];
    NodePtr grandchild = (*child)[0];

    if (grandchild->GetType() != "StringNode")
        return;

    literal = PtrCast<Spark::StringNode>(grandchild)->Get();

    children.clear();
    children.push_back(grandchild);
}

UnknownPtr<RValue> StringLiteralNode::Evaluate(CompileContext& context)
{
    std::string value = Parse(literal);
    int size = value.size();
    llvm::Constant* val = llvm::ConstantDataArray::getString(
        Spark::LLVMManager::Context(),
        value);

    auto* gvar = Spark::LLVMManager::Instance().CreateGlobalConstant(".str", val);

    LangType* type = context.builtins->types.Get("string");

    auto zero = Spark::TypeConverter::Create<int>(0);
    std::vector<llvm::Value*> indices { zero, zero };
    llvm::Value* str_ptr = context.builder.CreateGEP(gvar, indices, "str_ptr");
    llvm::Value* size_val = Spark::TypeConverter::Create<int>(size);
    llvm::Value* final_val = Spark::LLVMManager::Instance().CreateStructValue(context.builder, type->GetIR(), { size_val, str_ptr }, "str_literal");
    auto ptrVal = std::make_shared<GeneralRValue>(final_val, type);
    return PtrCast<RValue>(ptrVal);
}

/*static*/ std::string StringLiteralNode::Parse(std::string val)
{
    int len = val.size();
    char new_chars[len+1];

    int indexOld = 0;
    int indexNew = 0;

    while (indexOld < len)
    {
        char c = val[indexOld];
        if (c != '\\')
        {
            new_chars[indexNew] = c;
            indexNew++;
        }
        else
        {
            indexOld++;
            char modifier = val[indexOld];
            char value = GetSpecialValue(modifier);
            new_chars[indexNew] = value;
            indexNew++;
        }
        indexOld++;
    }

    new_chars[indexNew] = '\0';

    return std::string(new_chars);
}

/*static*/ char StringLiteralNode::GetSpecialValue(char val)
{
    switch (val)
    {
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case 'r':
            return '\r';
        case '\\':
            return '\\';
    }

    std::cerr << "Warning: unknown escape sequence '\\" << val << "', ignoring it." << std::endl;
    return val;
}


