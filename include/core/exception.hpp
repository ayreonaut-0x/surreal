#pragma once

#include "base.hpp"

#include <stdexcept>

namespace Surreal
{

class Exception
{
public:
    constexpr Exception() noexcept = default;
    constexpr Exception(const Exception&) noexcept = default;
    virtual ~Exception() noexcept = default;

    virtual const char* what() const noexcept = 0;
};

class LogicError : public Exception, public std::logic_error
{
public:
    explicit LogicError(const std::string& msg) : Exception(), std::logic_error(msg) {}
    explicit LogicError(const char* msg) : Exception(), std::logic_error(msg) {}

    const char* what() const noexcept override { return std::logic_error::what(); }
};

class RuntimeError : public Exception, public std::runtime_error
{
public:
    explicit RuntimeError(const std::string& msg) : Exception(), std::runtime_error(msg) {}
    explicit RuntimeError(const char* msg) : Exception(), std::runtime_error(msg) {}

    const char* what() const noexcept override { return std::runtime_error::what(); }
};

} // namespace Surreal
