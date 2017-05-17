#include "AST/Program.h"
#include "AST/Common.h"
#include "AST/Function.h"

RULE(ProgramPiece)
{
    Autoname(builder);
    builder.Add(FunctionRule, OptionalWhitespace, ProgramPiece);
    builder.Add(FunctionRule);

    builder.SetNodeType<ProgramPieceChain>();
}

RULE(Program)
{
    Autoname(builder);
    builder.Add(OptionalWhitespace, ProgramPiece, OptionalWhitespace);
    builder.Ignore(0);
    builder.Ignore(2);

    builder.SetNodeType<ProgramNode>();
}

void ProgramNode::Process()
{
    ConvertToOnlyCustom();

    // Recurse
    CustomNode::Process();
}

