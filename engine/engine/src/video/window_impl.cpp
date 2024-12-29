#include "nickel/video/internal/window_impl.hpp"

#include "nickel/common/log.hpp"

nickel::video::Window::Impl::Impl(const std::string& title, int w, int h) {
#ifdef NICKEL_PLATFORM_ANDROID
    m_window = SDL_CreateWindow(title.c_str(), 0, 0, SDL_WINDOW_VULKAN);
#else
    m_window = SDL_CreateWindow(title.c_str(), w, h, SDL_WINDOW_VULKAN);
#endif
    if (!m_window) {
        LOGC("create window failed");
    }
}

SDL_Window* nickel::video::Window::Impl::GetWindow() const {
    return m_window;
}