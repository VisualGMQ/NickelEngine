#include "nickel/common/log.hpp"

#include "nickel/common/sdl_call.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel {

LogSystem::LogSystem() {
    SDL_Time time;
    SDL_CALL(SDL_GetCurrentTime(&time));
    SDL_DateTime dt;
    SDL_CALL(SDL_TimeToDateTime(time, &dt, true));

    std::string log_filename =
        fmt::format("log/{}-{}-{}_{}-{}-{}.log", dt.year, dt.month, dt.day,
                    dt.hour, dt.minute, dt.second);
#ifdef NICKEL_PLATFORM_ANDROID
    m_console_logger = spdlog::android_logger_mt("android", "nickelengine");
    log_filename =
        std::string(SDL_GetAndroidInternalStoragePath()) + '/' + log_filename;
#else
    m_console_logger = spdlog::stdout_color_mt("console");
#endif
    m_file_logger = spdlog::basic_logger_mt("file_logger", log_filename);
}

}  // namespace nickel