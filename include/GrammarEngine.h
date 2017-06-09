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
        DebugContext debugContext;
    public:
        GrammarEngine(std::istream& input);
        NodePtr Start(RuleFuncWrapper func);
        void SetDebug(bool val);
    };
}
