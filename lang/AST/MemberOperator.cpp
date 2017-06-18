#include "AST/MemberOperator.h"

#include "AST/Common.h"
#include "Operator.h"

RULE(MemberOperator)
{
    Autoname(builder);
    builder.Add('.', OptionalWhitespace, Identifier);

    builder.SetNodeType<MemberOperatorNode>();
}

UnknownPtr<RValue> MemberOperatorNode::Create(UnknownPtr<RValue> lhs, CompileContext& context)
{
    auto idNode = SafeGet<IdentifierNode>(0, "IdentifierNode");
    std::string name = idNode->GetValue();

    GetMemberOperator op(name);
    return op.Create(lhs, context);
}


