#pragma once

#include <iostream>

#include "InputBuffer.h"
#include "Delegates.h"
#include "Node.h"

namespace Spark
{
    class GrammarEngine
    {
    private:
        InputBuffer buffer;
    public:
        GrammarEngine(std::istream& input);
        NodePtr Start(RuleFuncWrapper func);
    };
}
