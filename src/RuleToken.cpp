#include "RuleToken.h"

#include <sstream>

using namespace Spark;

RuleToken::RuleToken() : mode(CHAR_MODE), isAssigned(false) {}

// Big 5
RuleToken::RuleToken(const RuleToken& other) : mode(other.mode), isAssigned(other.isAssigned)
{
    if (!other.isAssigned)
        return;

    DoNewAssign(other);
}

RuleToken::RuleToken(RuleToken&& other) : mode(other.mode), isAssigned(other.isAssigned)
{
    if (!other.isAssigned)
        return;

    DoNewAssign(std::move(other));
}

RuleToken& RuleToken::operator=(const RuleToken& other)
{
    if (other.isAssigned)
    {
        if (isAssigned && mode == other.mode)
        {
            DoDirectAssign(other);
        }
        else
        {
            DestroyAll();
            DoNewAssign(other);
        }
    }
    else
    {
        DestroyAll();
    }

    isAssigned = other.isAssigned;
    mode = other.mode;

    return *this;
}

RuleToken& RuleToken::operator=(RuleToken&& other)
{
    if (other.isAssigned)
    {
        if (isAssigned && mode == other.mode)
        {
            DoDirectAssign(std::move(other));
        }
        else
        {
            DestroyAll();
            DoNewAssign(std::move(other));
        }
    }
    else
    {
        DestroyAll();
    }

    isAssigned = other.isAssigned;
    mode = other.mode;

    return *this;
}

RuleToken::~RuleToken()
{
    DestroyAll();
}

void RuleToken::Set(char c)
{
    if (isAssigned)
        throw RuleTokenException("Token already initialized.");

    isAssigned = true;
    mode = CHAR_MODE;
    charVal = c;
}

void RuleToken::Set(std::string str)
{
    if (isAssigned)
        throw RuleTokenException("Token already initialized.");

    isAssigned = true;
    mode = STRING_MODE;
    // placement new
    new(&stringVal) std::string(str);
}

void RuleToken::Set(RuleFuncWrapper func)
{
    if (isAssigned)
        throw RuleTokenException("Token already initialized.");

    isAssigned = true;
    mode = FUNC_MODE;
    // placement new
    new(&funcVal) RuleFuncWrapper(func);
}

void RuleToken::Set(CharsetPredicate charset)
{
    if (isAssigned)
        throw RuleTokenException("Token already initialized.");

    isAssigned = true;
    mode = CHARSET_MODE;
    // placement new
    new(&charsetVal) CharsetPredicate(charset);
}

char RuleToken::GetChar() const
{
    if (!isAssigned)
        throw RuleTokenException("Token not initialized");

    if (mode != CHAR_MODE)
    {
        std::stringstream ss;
        ss << "Wrong type: Token is not a char. Token is " << mode;
        throw RuleTokenException(ss.str());
    }

    return charVal;
}

std::string RuleToken::GetString() const
{
    if (!isAssigned)
        throw RuleTokenException("Token not initialized");

    if (mode != STRING_MODE)
        throw RuleTokenException("Wrong type: Token is not a string");

    return stringVal;
}

RuleFuncWrapper RuleToken::GetFunc() const
{
    if (!isAssigned)
        throw RuleTokenException("Token not initialized");

    if (mode != FUNC_MODE)
        throw RuleTokenException("Wrong type: Token is not a func");

    return funcVal;
}

CharsetPredicate RuleToken::GetCharset() const
{
    if (!isAssigned)
        throw RuleTokenException("Token not initialized");

    if (mode != CHARSET_MODE)
        throw RuleTokenException("Wrong type: Token is not a charset");

    return charsetVal;
}

void RuleToken::DoDirectAssign(const RuleToken& other)
{
    switch (mode)
    {
        case CHAR_MODE:
            charVal = other.charVal;
            break;
        case STRING_MODE:
            stringVal = other.stringVal;
            break;
        case FUNC_MODE:
            funcVal = other.funcVal;
            break;
        case CHARSET_MODE:
            charsetVal = other.charsetVal;
            break;
        default:
            throw RuleTokenException("UNREACHABLE");
    }
}

void RuleToken::DoDirectAssign(RuleToken&& other)
{
    switch (mode)
    {
        case CHAR_MODE:
            charVal = other.charVal;
            break;
        case STRING_MODE:
            stringVal = std::move(other.stringVal);
            break;
        case FUNC_MODE:
            funcVal = std::move(other.funcVal);
            break;
        case CHARSET_MODE:
            charsetVal = std::move(other.charsetVal);
            break;
        default:
            throw RuleTokenException("UNREACHABLE");
    }
}


void RuleToken::DoNewAssign(const RuleToken& other)
{
    switch (other.mode)
    {
        case CHAR_MODE:
            charVal = other.charVal;
            break;
        case STRING_MODE:
            new(&stringVal) std::string(other.stringVal);
            break;
        case FUNC_MODE:
            new(&funcVal) RuleFuncWrapper(other.funcVal);
            break;
        case CHARSET_MODE:
            new(&charsetVal) CharsetPredicate(other.charsetVal);
            break;
        default:
            throw RuleTokenException("UNREACHABLE");
    }
}

void RuleToken::DoNewAssign(RuleToken&& other)
{
    switch (other.mode)
    {
        case CHAR_MODE:
            charVal = other.charVal;
            break;
        case STRING_MODE:
            new(&stringVal) std::string(std::move(other.stringVal));
            break;
        case FUNC_MODE:
            new(&funcVal) RuleFuncWrapper(std::move(other.funcVal));
            break;
        case CHARSET_MODE:
            new(&charsetVal) CharsetPredicate(std::move(other.charsetVal));
            break;
        default:
            throw RuleTokenException("UNREACHABLE");
    }
}

void RuleToken::DestroyAll()
{
    if (!isAssigned)
        return;

    switch (mode)
    {
        case STRING_MODE:
            using std::string;
            stringVal.~string();
            break;
        case FUNC_MODE:
            funcVal.~RuleFuncWrapper();
            break;
        case CHARSET_MODE:
            charsetVal.~CharsetPredicate();
        default:
            break;
    }
}


