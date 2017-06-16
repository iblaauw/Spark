#pragma once

#include <map>

#include "OperatorImpl.h"

class LangType;

class MemberTable
{
public:
    std::map<std::string, UnaryOperatorImpl*> unaryOperators;
    std::map<std::string, std::map<LangType*, BinaryOperatorImpl*>> binaryOperators;
    SpecialOperatorImpl* indexOperator;
    SpecialOperatorImpl* callOperator;
};


