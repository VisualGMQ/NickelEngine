﻿#pragma once
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
        ::nickel::LogSystem::GetInst().m_console_logger->error(fmt,            \
                                                               ##__VA_ARGS__); \
        ::nickel::LogSystem::GetInst().m_file_logger->error(fmt,               \
                                                            ##__VA_ARGS__);    \
    } while (0)

#define LOGW(fmt, ...)                                                        \
    do {                                                                      \
        ::nickel::LogSystem::GetInst().m_console_logger->warn(fmt,            \
                                                              ##__VA_ARGS__); \
        ::nickel::LogSystem::GetInst().m_file_logger->warn(fmt,               \
                                                           ##__VA_ARGS__);    \
    } while (0)

#define LOGD(fmt, ...)                                                         \
    do {                                                                       \
        ::nickel::LogSystem::GetInst().m_console_logger->debug(fmt,            \
                                                               ##__VA_ARGS__); \
        ::nickel::LogSystem::GetInst().m_file_logger->debug(fmt,               \
                                                            ##__VA_ARGS__);    \
    } while (0)

#define LOGI(fmt, ...)                                                        \
    do {                                                                      \
        ::nickel::LogSystem::GetInst().m_console_logger->info(fmt,            \
                                                              ##__VA_ARGS__); \
        ::nickel::LogSystem::GetInst().m_file_logger->info(fmt,               \
                                                           ##__VA_ARGS__);    \
    } while (0)

#define LOGT(fmt, ...)                                                         \
    do {                                                                       \
        ::nickel::LogSystem::GetInst().m_console_logger->trace(fmt,            \
                                                               ##__VA_ARGS__); \
        ::nickel::LogSystem::GetInst().m_file_logger->trace(fmt,               \
                                                            ##__VA_ARGS__);    \
    } while (0)

#define LOGC(fmt, ...)                                                         \
    do {                                                                       \
        ::nickel::LogSystem::GetInst().m_console_logger->critical(             \
            fmt, ##__VA_ARGS__);                                               \
        ::nickel::LogSystem::GetInst().m_file_logger->critical(fmt,            \
                                                               ##__VA_ARGS__); \
        assert(false);                                                         \
    } while (0)

}  // namespace nickel