#pragma once

#include <map>

#include "OperatorImpl.h"

class MemberTable
{
public:
    std::map<std::string, UnaryOperatorImpl*> unaryOperators;
    std::map<std::string, BinaryOperatorImpl*> binaryOperators;
    SpecialOperatorImpl* indexOperator;
    SpecialOperatorImpl* callOperator;
};


