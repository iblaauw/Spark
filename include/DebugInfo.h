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

    class DebugContext
    {
    private:
        DebugInfo maxInfo;
        std::vector<std::string> trace;
        std::vector<std::string> maxTrace;
        bool tracking;

    public:
        bool debugEnabled;

        DebugContext() : tracking(false), debugEnabled(true) {}
        void SetPosition(DebugInfo info);
        void AddTrace(std::string trace);
        void PopTrace();

        std::string GetErrorMessage() const;
        std::string GetErrorMessage(std::istream& input) const;
    private:
        std::string DoGetMessage() const;
        std::string DoGetStack() const;
    };

}
