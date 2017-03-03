#pragma once

#include <functional>

namespace Spark
{
    class IRuleBuilder;
    using RuleFuncWrapper = std::function<void(IRuleBuilder&)>;
    using CharsetPredicate = std::function<bool(char)>;
}
