#pragma once

#include "IRuleBuilder.h"



#define RULE(name) \
void name(Spark::IRuleBuilder& builder)

#define CHARSET(name) \
bool name(char c)

CHARSET(Alpha);
CHARSET(OperatorChar);
CHARSET(NumberChar);
CHARSET(WhitespaceChar);
CHARSET(StringLiteralChar);

RULE(Whitespace);
RULE(OptionalWhitespace);
RULE(Identifier);
RULE(Type);
RULE(Number);
RULE(StringLiteral);
RULE(FuncCall);
RULE(Expression);
RULE(Statement);
RULE(StatementBlock);
RULE(ProgramPiece);
RULE(Program);

// Function Declarations
RULE(Function);
RULE(FunctionParameterList);
RULE(FunctionParameter);


