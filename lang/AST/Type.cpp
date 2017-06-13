#include "AST/Type.h"

#include "AST/Common.h"
#include "AST/NumberLiteral.h"

class TypeModifierNode : public CustomNode
{
public:
    TypeModifierNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "TypeModifierNode"; }

    virtual LangType* Modify(LangType* original) = 0;
};

class TypeModifierSetNode : public ChainingNode
{
public:
    TypeModifierSetNode(std::vector<NodePtr>& nodes) : ChainingNode(nodes) {}
    std::string GetType() const override { return "TypeModifierSetNode"; }

    LangType* Modify(LangType* original);
};

/// Type Modifiers  ///

class _PtrModifierNode : public TypeModifierNode {
public:
    _PtrModifierNode(std::vector<NodePtr>& nodes) : TypeModifierNode(nodes) {}
    LangType* Modify(LangType* lt) { return lt->GetPointerTo(); }
};

static RULE(_PtrModifier)
{
    Autoname(builder);
    builder.Add('*');
    builder.SetNodeType<_PtrModifierNode>();
}

class _ArrayModifierNode : public TypeModifierNode {
public:
    _ArrayModifierNode(std::vector<NodePtr>& nodes) : TypeModifierNode(nodes) {}
    LangType* Modify(LangType* lt) {
        auto numberLiteral = SafeGet<NumberLiteralNode>(0, "NumberLiteralNode");
        int val = numberLiteral->GetNumber();
        return lt->GetArrayOf(val);
    }
};

static RULE(_ArrayModifier)
{
    Autoname(builder);
    builder.Add('[', OptionalWhitespace, NumberLiteral, OptionalWhitespace, ']');
    builder.Ignore(1);
    builder.Ignore(3);

    builder.SetNodeType<_ArrayModifierNode>();

}

/// Type Modifier Choice ///

class _TypeModifierChoiceNode : public CustomNode
{
public:
    _TypeModifierChoiceNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "_TypeModifierChoiceNode"; }
    LangType* Modify(LangType* original)
    {
        auto modifier = SafeGet<TypeModifierNode>(0, "TypeModifierNode");
        return modifier->Modify(original);
    }
};

RULE(TypeModifier)
{
    Autoname(builder);
    builder.Add(_PtrModifier);
    builder.Add(_ArrayModifier);
    builder.SetNodeType<_TypeModifierChoiceNode>();
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

LangType* TypeModifierSetNode::Modify(LangType* original)
{
    for (unsigned int i = 0; i < customChildren.size(); i++)
    {
        auto modifier = SafeGet<_TypeModifierChoiceNode>(i, "_TypeModifierChoiceNode");
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
