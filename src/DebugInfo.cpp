#include "DebugInfo.h"

#include <sstream>
#include <istream>

namespace Spark
{
    void DebugInfo::Advance(char c)
    {
        if (c == '\n')
        {
            lineNum++;
            charNum = -1;
        }
        else
        {
            charNum++;
        }
    }

    std::string DebugInfo::GetErrorMessage(std::string message) const
    {
        std::stringstream ss;
        // Note: these are 0 indexed but for readability make 1 indexed
        ss << "At line " << lineNum + 1 << " char " << charNum + 1 << ": " << message << std::endl;

        return ss.str();
    }

    std::string DebugInfo::GetErrorMessage(std::string mess, std::istream& input) const
    {
        std::string message = GetErrorMessage(mess);

        auto pos = input.tellg();

        /// Get the line the error occurred at
        input.seekg(0); // rewind
        std::string line;
        for (int i = 0; i <= lineNum; i++)
        {
            getline(input, line);
        }

        input.seekg(pos);

        message += line;

        std::string spaces(charNum+1, ' ');
        message += "\n" + spaces + "^\n";

        return message;
    }


    bool DebugInfo::operator<(const DebugInfo& other) const
    {
        if (other.lineNum == lineNum)
        {
            return charNum < other.charNum;
        }

        return lineNum < other.lineNum;
    }

    bool DebugInfo::operator==(const DebugInfo& other) const
    {
        return (lineNum == other.lineNum) && (charNum == other.charNum);
    }

    bool DebugInfo::operator>(const DebugInfo& other) const
    {
        if (other.lineNum == lineNum)
        {
            return charNum > other.charNum;
        }

        return lineNum > other.lineNum;
    }


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
        std::string message = maxInfo.GetErrorMessage("Invalid Syntax");
        if (debugEnabled)
            message += DoGetStack();
        return message;
    }

    std::string DebugContext::GetErrorMessage(std::istream& input) const
    {
        std::string message = maxInfo.GetErrorMessage("Invalid Syntax", input);

        if (debugEnabled)
            message += DoGetStack();

        return message;
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

