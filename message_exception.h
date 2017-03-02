#pragma once

#include <exception>
#include <string>

class MessageException : public std::exception
{
private:
    std::string message;
public:
    MessageException(const char* message) : message(message) {}
    MessageException(std::string message) : message(message) {}
    const char* what() const throw() override { return message.c_str(); }
};

#define DECL_EXCEPTION(_ExceptionName) \
class _ExceptionName : public MessageException \
{ \
public: \
    _ExceptionName (const char* m) : MessageException(m) {} \
    _ExceptionName (const std::string& m) : MessageException(m) {} \
}

DECL_EXCEPTION(__Death);

#define _DEATH(FILE_, LINE) do { throw __Death("Fatal error at " #FILE_ ", line " #LINE); } while(0)

#define _DEATH_PROXY(FILE_, LINE) _DEATH(FILE_, LINE)

#define DEATH() _DEATH_PROXY(__FILE__, __LINE__)

