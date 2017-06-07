#include "DebugInfo.h"

#include <sstream>

namespace Spark
{
    std::string FailInfo::GetErrorMessage() const
    {
        std::stringstream ss;
        ss << "Invalid syntax at line " << debugInfo.lineNum << " char " << debugInfo.charNum << std::endl;
        ss << "Syntax Tree Trace:" << std::endl;

        for (std::string name : trace)
        {
            ss << "    " << name << std::endl;
        }

        return ss.str();
    }

    void FailInfo::AdvanceForwardTo(FailInfo&& other)
    {
        if (!(debugInfo < other.debugInfo))
            return;

        debugInfo = other.debugInfo;
        trace = std::move(other.trace);
    }

    void FailInfo::Clear()
    {
        trace.clear();
        debugInfo.lineNum = 0;
        debugInfo.charNum = 0;
    }
}

