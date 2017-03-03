#pragma once

#include <iostream>

#include "InputBuffer.h"
#include "Delegates.h"

namespace Spark
{
    class GrammarEngine
    {
    private:
        InputBuffer buffer;
    public:
        GrammarEngine(std::istream& input);
        void Start(RuleFuncWrapper func);
    };
}
