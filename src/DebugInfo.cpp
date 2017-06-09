#include "DebugInfo.h"

#include <sstream>

namespace Spark
{
    /// DebugContext ///

    void DebugContext::SetPosition(DebugInfo info)
    {
        if (!tracking && info > maxInfo)
        {
            maxTrace = trace;
            tracking = true;
        }
        else if (tracking && info < maxInfo)
        {
            tracking = false;
        }

        if (tracking)
        {
            maxInfo = info;
        }
    }

    void DebugContext::AddTrace(std::string trace)
    {
        this->trace.push_back(trace);
        if (tracking)
            maxTrace.push_back(trace);
    }

    void DebugContext::PopTrace()
    {
        this->trace.pop_back();
        tracking = false;
    }

    std::string DebugContext::GetErrorMessage() const
    {
        std::stringstream ss;
        // Note: these are 0 indexed but for readability make 1 indexed
        ss << "Invalid syntax at line " << maxInfo.lineNum + 1 << " char " << maxInfo.charNum + 1 << std::endl;
        ss << "Syntax Tree Trace:" << std::endl;

        for (std::string name : maxTrace)
        {
            ss << "    " << name << std::endl;
        }

        return ss.str();
    }

}

