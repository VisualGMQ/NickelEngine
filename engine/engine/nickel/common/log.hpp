#pragma once
#include "nickel/common/singleton.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#ifdef NICKEL_PLATFORM_ANDROID
#include "spdlog/sinks/android_sink.h"
#endif
#include "nickel/common/dllexport.hpp"
#include "spdlog/spdlog.h"

namespace nickel {

class NICKEL_API LogSystem : public Singlton<LogSystem, false> {
public:
    LogSystem();

    std::shared_ptr<spdlog::logger> m_console_logger;
    std::shared_ptr<spdlog::logger> m_file_logger;
};

#define LOGE(fmt, ...)                                                         \
    do {                                                                       \
        SPDLOG_LOGGER_ERROR(::nickel::LogSystem::GetInst().m_console_logger,   \
                            fmt, ##__VA_ARGS__);                               \
        SPDLOG_LOGGER_ERROR(::nickel::LogSystem::GetInst().m_file_logger, fmt, \
                            ##__VA_ARGS__);                                    \
    } while (0)

#define LOGW(fmt, ...)                                                        \
    do {                                                                      \
        SPDLOG_LOGGER_WARN(::nickel::LogSystem::GetInst().m_console_logger,   \
                           fmt, ##__VA_ARGS__);                               \
        SPDLOG_LOGGER_WARN(::nickel::LogSystem::GetInst().m_file_logger, fmt, \
                           ##__VA_ARGS__);                                    \
    } while (0)

#define LOGD(fmt, ...)                                                         \
    do {                                                                       \
        SPDLOG_LOGGER_DEBUG(::nickel::LogSystem::GetInst().m_console_logger,   \
                            fmt, ##__VA_ARGS__);                               \
        SPDLOG_LOGGER_DEBUG(::nickel::LogSystem::GetInst().m_file_logger, fmt, \
                            ##__VA_ARGS__);                                    \
    } while (0)

#define LOGI(fmt, ...)                                                        \
    do {                                                                      \
        SPDLOG_LOGGER_INFO(::nickel::LogSystem::GetInst().m_console_logger,   \
                           fmt, ##__VA_ARGS__);                               \
        SPDLOG_LOGGER_INFO(::nickel::LogSystem::GetInst().m_file_logger, fmt, \
                           ##__VA_ARGS__);                                    \
    } while (0)

#define LOGT(fmt, ...)                                                         \
    do {                                                                       \
        SPDLOG_LOGGER_TRACE(::nickel::LogSystem::GetInst().m_console_logger,   \
                            fmt, ##__VA_ARGS__);                               \
        SPDLOG_LOGGER_TRACE(::nickel::LogSystem::GetInst().m_file_logger, fmt, \
                            ##__VA_ARGS__);                                    \
    } while (0)

#define LOGC(fmt, ...)                                                       \
    do {                                                                     \
        SPDLOG_LOGGER_CRITICAL(                                              \
            ::nickel::LogSystem::GetInst().m_console_logger, fmt,            \
            ##__VA_ARGS__);                                                  \
        SPDLOG_LOGGER_CRITICAL(::nickel::LogSystem::GetInst().m_file_logger, \
                               fmt, ##__VA_ARGS__);                          \
        assert(false);                                                       \
    } while (0)

}  // namespace nickel