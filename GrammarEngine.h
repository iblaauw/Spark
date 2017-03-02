#pragma once

#include <iostream>

#include "InputBuffer.h"
#include "RuleFunc.h"
//#include "PathManager.h"

namespace Spark
{
    class GrammarEngine
    {
    private:
        InputBuffer buffer;
        //PathManager path;
    public:
        GrammarEngine(std::istream& input);
        void Start(RuleFunc func);
    };
}
