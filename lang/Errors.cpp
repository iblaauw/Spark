#include "Errors.h"

static bool _errorFlag = false;

bool HasError() { return _errorFlag; }

void SetError() { _errorFlag = true; }

