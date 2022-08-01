#include <core/log.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Surreal
{

std::shared_ptr<spdlog::logger> Log::s_logger{};

void Log::init()
{
    std::array<spdlog::sink_ptr, 2ul> sinks{ {
        { std::make_shared<spdlog::sinks::stdout_color_sink_mt>() },
        { std::make_shared<spdlog::sinks::basic_file_sink_mt>("Surreal.log", true) },
    } };

    sinks[0]->set_pattern("[%T] %^[%l] %v");
    sinks[1]->set_pattern("[%T] [%l] %v");

    s_logger = std::make_shared<spdlog::logger>("SURREAL", sinks.begin(), sinks.end());
    s_logger->set_level(spdlog::level::trace);
    s_logger->flush_on(spdlog::level::trace);
    spdlog::register_logger(s_logger);
}

} // namespace Surreal
