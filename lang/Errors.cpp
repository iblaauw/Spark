#include "Errors.h"

#include <istream>
#include <execinfo.h>

static bool _errorFlag = false;
static std::istream* _currentFile;

bool HasError() { return _errorFlag; }

void SetError() { _errorFlag = true; }

void SetCurrentFile(std::istream& input) { _currentFile = &input; }
std::istream& GetCurrentFile() { return *_currentFile; }

void PrintStackTrace(std::ostream& out)
{
    void* buffer[100];
    int buf_size = backtrace(buffer, 100);

    char** traces = backtrace_symbols(buffer, buf_size);

    out << "~~~ Printing Backtrace ~~~" << std::endl;
    for (int i = 0; i < buf_size; i++)
    {
        out << traces[i] << std::endl;
    }

    free(traces);
}

