#include <core/log.hpp>

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Surreal
{

std::shared_ptr<spdlog::async_logger> Log::s_logger{};

void Log::init()
{
    spdlog::init_thread_pool(8192ul, 1ul);

    auto console_sink{ std::make_shared<spdlog::sinks::stdout_color_sink_mt>() };
    console_sink->set_pattern("%^[%T] [%l]%$ %v");
    console_sink->set_level(spdlog::level::warn);

    auto file_sink{ std::make_shared<spdlog::sinks::basic_file_sink_mt>("Surreal.log", true) };
    file_sink->set_pattern("[%T] [%l] %v");
    file_sink->set_level(spdlog::level::trace);

    s_logger = std::make_shared<spdlog::async_logger>("SURREAL", spdlog::sinks_init_list{ console_sink, file_sink },
                                                      spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    s_logger->flush_on(spdlog::level::trace);
    spdlog::register_logger(s_logger);
}

} // namespace Surreal
