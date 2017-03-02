#include "RuleToken.h"

using namespace Spark;

RuleToken::RuleToken() : isFunc(false), isAssigned(false) {}

// Big 5
RuleToken::RuleToken(const RuleToken& other) : isFunc(other.isFunc), isAssigned(other.isAssigned)
{
    if (!other.isAssigned)
        return;

    if (other.isFunc)
    {
        new(&funcVal) RuleFuncWrapper(other.funcVal);
    }
    else
    {
        charVal = other.charVal;
    }
}

RuleToken::RuleToken(RuleToken&& other) : isFunc(other.isFunc), isAssigned(other.isAssigned)
{
    if (!other.isAssigned)
        return;

    if (other.isFunc)
    {
        new(&funcVal) RuleFuncWrapper(std::move(other.funcVal));
    }
    else
    {
        charVal = other.charVal;
    }
}

RuleToken& RuleToken::operator=(const RuleToken& other)
{
    if (other.isAssigned)
    {
        if (isFunc && other.isFunc)
        {
            funcVal = other.funcVal;
        }
        else if (isFunc && !other.isFunc)
        {
            funcVal.~RuleFuncWrapper();
            charVal = other.charVal;
        }
        else if (!isFunc && other.isFunc)
        {
            new(&funcVal) RuleFuncWrapper(other.funcVal);
        }
        else
        {
            charVal = other.charVal;
        }
    }
    else
    {
        if (isFunc)
        {
            funcVal.~RuleFuncWrapper();
        }
    }

    isAssigned = other.isAssigned;
    isFunc = other.isFunc;

    return *this;
}

RuleToken& RuleToken::operator=(RuleToken&& other)
{
    if (other.isAssigned)
    {
        if (isFunc && other.isFunc)
        {
            funcVal = std::move(other.funcVal);
        }
        else if (isFunc && !other.isFunc)
        {
            funcVal.~RuleFuncWrapper();
            charVal = other.charVal;
        }
        else if (!isFunc && other.isFunc)
        {
            new(&funcVal) RuleFuncWrapper(std::move(other.funcVal));
        }
        else
        {
            charVal = other.charVal;
        }
    }
    else
    {
        if (isFunc)
        {
            funcVal.~RuleFuncWrapper();
        }
    }

    isAssigned = other.isAssigned;
    isFunc = other.isFunc;

    return *this;
}

RuleToken::~RuleToken()
{
    if (isFunc)
    {
        funcVal.~RuleFuncWrapper();
    }
}

void RuleToken::Set(char c)
{
    if (isAssigned)
        throw RuleTokenException("Token already initialized.");

    isAssigned = true;
    isFunc = false;
    charVal = c;
}

void RuleToken::Set(RuleFuncWrapper func)
{
    if (isAssigned)
        throw RuleTokenException("Token already initialized.");

    isAssigned = true;
    isFunc = true;
    // placement new
    new(&funcVal) RuleFuncWrapper(func);
}

char RuleToken::GetChar() const
{
    if (!isAssigned)
        throw RuleTokenException("Token not initialized");

    if (isFunc)
        throw RuleTokenException("Token is wrong type");

    return charVal;
}

RuleFuncWrapper RuleToken::GetFunc() const
{
    if (!isAssigned)
        throw RuleTokenException("Token not initialized");

    if (!isFunc)
        throw RuleTokenException("Token is wrong type");

    return funcVal;
}


