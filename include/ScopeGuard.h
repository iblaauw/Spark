#pragma once

#include <functional>
#include <exception>

class ScopeGuard
{
private:
    std::function<void()> func;
public:
    ScopeGuard(std::function<void()> func) : func(func) {}
    ~ScopeGuard() { func(); }
};

class ScopeGuardNormal
{
private:
    std::function<void()> func;
public:
    ScopeGuardNormal(std::function<void()> func) : func(func) {}
    ~ScopeGuardNormal()
    {
        if (!std::uncaught_exception())
            func();
    }
};


#define ON_EXIT() \
    ScopeGuard _exit_guard_ ## __LINE__ = [=]()

#define ON_EXIT_NORMAL() \
    ScopeGuardNormal _exit_guard_ ## __LINE__ = [=]()

