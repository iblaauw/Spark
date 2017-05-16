#include "LangType.h"


LangType::LangType(std::string name) : name(name) {}

bool LangType::IsAssignableFrom(LangType& otherType) const
{
    return name == otherType.name;
}

void LangType::InsertConversion(LangType& fromType, CompileContext& context) const
{
    // NOOP (default will not need to convert)
}

