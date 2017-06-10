#include "Errors.h"

#include <istream>

static bool _errorFlag = false;
static std::istream* _currentFile;

bool HasError() { return _errorFlag; }

void SetError() { _errorFlag = true; }

void SetCurrentFile(std::istream& input) { _currentFile = &input; }
std::istream& GetCurrentFile() { return *_currentFile; }

