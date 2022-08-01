#pragma once

#include "base.hpp"

#include <spdlog/spdlog.h>

namespace Surreal
{

class SURREAL_API_EXPORT Log
{
public:
    static constexpr std::shared_ptr<spdlog::logger>& get() noexcept { return s_logger; }

    static void init();

private:
    static std::shared_ptr<spdlog::logger> s_logger;
};

} // namespace Surreal

#define SURREAL_LOG_INFO(...) ::Surreal::Log::get()->info(__VA_ARGS__)
#define SURREAL_LOG_WARN(...) ::Surreal::Log::get()->warn(__VA_ARGS__)
#define SURREAL_LOG_ERROR(...) ::Surreal::Log::get()->error(__VA_ARGS__)
