#include "DebugInfo.h"

#include <sstream>
#include <istream>

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
        std::string message = DoGetMessage();
        if (debugEnabled)
            message += DoGetStack();
        return message;
    }

    std::string DebugContext::GetErrorMessage(std::istream& input) const
    {
        std::string message = DoGetMessage();

        /// Get the line the error occurred at
        input.seekg(0); // rewind
        std::string line;
        for (int i = 0; i <= maxInfo.lineNum; i++)
        {
            getline(input, line);
        }

        message += line;

        std::string spaces(maxInfo.charNum, ' ');
        message += "\n" + spaces + "^\n";


        if (debugEnabled)
            message += DoGetStack();

        return message;
    }

    std::string DebugContext::DoGetMessage() const
    {
        std::stringstream ss;
        // Note: these are 0 indexed but for readability make 1 indexed
        ss << "Invalid syntax at line " << maxInfo.lineNum + 1 << " char " << maxInfo.charNum + 1 << std::endl;

        return ss.str();
    }

    std::string DebugContext::DoGetStack() const
    {
        std::stringstream ss;
        ss << "Syntax Tree Trace:" << std::endl;

        for (std::string name : maxTrace)
        {
            if (name.size() > 0)
                ss << "    " << name << std::endl;
        }

        return ss.str();
    }

}

