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

RULE(OptionalWhitespace);
RULE(Identifier);
RULE(Number);
RULE(Expression);
RULE(Function);
RULE(ProgramPiece);
RULE(Program);


