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

        void Advance(char c);
        std::string GetErrorMessage(std::string message) const;
        std::string GetErrorMessage(std::string message, std::istream& input) const;

        bool operator<(const DebugInfo& other) const;

        bool operator==(const DebugInfo& other) const;

        bool operator>(const DebugInfo& other) const;
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

        DebugContext() : tracking(false), debugEnabled(false) {}
        void SetPosition(DebugInfo info);
        void AddTrace(std::string trace);
        void PopTrace();

        std::string GetErrorMessage() const;
        std::string GetErrorMessage(std::istream& input) const;
    private:
        std::string DoGetStack() const;
    };

}
