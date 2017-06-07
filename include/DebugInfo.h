#pragma once

#include <vector>
#include <string>

namespace Spark
{
    class DebugInfo
    {
    public:
        int lineNum = 0;
        int charNum = 0;

        void Advance(char c)
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

        bool operator<(const DebugInfo& other) const
        {
            if (other.lineNum == lineNum)
            {
                return charNum < other.charNum;
            }

            return lineNum < other.lineNum;
        }

        bool operator==(const DebugInfo& other) const
        {
            return (lineNum == other.lineNum) && (charNum == other.charNum);
        }

        bool operator>(const DebugInfo& other) const
        {
            if (other.lineNum == lineNum)
            {
                return charNum > other.charNum;
            }

            return lineNum > other.lineNum;
        }

    };

    class FailInfo
    {
    public:
        DebugInfo debugInfo;
        std::vector<std::string> trace;

        std::string GetErrorMessage() const;
        void AdvanceForwardTo(FailInfo&& other);
        void Clear();
    };

}
