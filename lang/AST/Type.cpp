#include "AST/Type.h"

#include "AST/Common.h"

class TypeModifierNode : public StringValueNode
{
public:
    TypeModifierNode(std::vector<NodePtr>& nodes) : StringValueNode(nodes) {}
    std::string GetType() const override { return "TypeModifierNode"; }

    UnknownPtr<LangType> Modify(UnknownPtr<LangType> original);
};

class TypeModifierSetNode : public ChainingNode
{
public:
    TypeModifierSetNode(std::vector<NodePtr>& nodes) : ChainingNode(nodes) {}
    std::string GetType() const override { return "TypeModifierSetNode"; }

    UnknownPtr<LangType> Modify(UnknownPtr<LangType> original);
};

RULE(TypeModifier)
{
    Autoname(builder);
    builder.Add("*");
    builder.SetNodeType<TypeModifierNode>();
}

RULE(TypeModifierSet)
{
    Autoname(builder);
    builder.Add(TypeModifier, TypeModifierSet);
    builder.AddEmpty();
    builder.SetNodeType<TypeModifierSetNode>();
}

RULE(Type)
{
    Autoname(builder);
    builder.Add(Identifier, TypeModifierSet); // Note: NO whitespace allowed here

    builder.SetNodeType<TypeNode>();
}

UnknownPtr<LangType> TypeModifierNode::Modify(UnknownPtr<LangType> original)
{
    std::string val = GetValue();
    if (val == "*")
    {
        return original->GetPointerTo();
    }

    Assert(false, "Unknown type modifier '", val, "'");
    return nullptr;
}

UnknownPtr<LangType> TypeModifierSetNode::Modify(UnknownPtr<LangType> original)
{
    for (unsigned int i = 0; i < customChildren.size(); i++)
    {
        auto modifier = SafeGet<TypeModifierNode>(i, "TypeModifierNode");
        original = modifier->Modify(original);
    }
    return original;
}

void TypeNode::VerifyTypes(CompileContext& context)
{
    auto baseNode = SafeGet<IdentifierNode>(0, "IdentifierNode");
    auto modifierSet = SafeGet<TypeModifierSetNode>(1, "TypeModifierSetNode");

    std::string baseName = baseNode->GetValue();

    LangType* originalType = context.symbolTable->types.Get(baseName);

    if (originalType == nullptr)
    {
        Error("unknown type '", baseName, "'");
        return;
    }

    this->type = modifierSet->Modify(originalType);
}
