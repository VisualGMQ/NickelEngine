#include "nickel/video/internal/window_impl.hpp"

#include "nickel/common/log.hpp"

namespace nickel::video {

Window::Impl::Impl(const std::string& title, int w, int h) {
#ifdef NICKEL_PLATFORM_ANDROID
    m_window = SDL_CreateWindow(title.c_str(), 0, 0, SDL_WINDOW_VULKAN);
#else
    m_window = SDL_CreateWindow(title.c_str(), w, h, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
#endif
    if (!m_window) {
        LOGC("create window failed");
    }
}

SDL_Window* Window::Impl::GetWindow() const {
    return m_window;
}

SVector<uint32_t, 2> Window::Impl::GetSize() const noexcept {
    int w, h;
    SDL_GetWindowSize(m_window, &w, &h);
    return SVector<uint32_t, 2>(w, h);
}

bool Window::Impl::IsMinimize() const noexcept {
    return SDL_GetWindowFlags(m_window) & SDL_WINDOW_MINIMIZED;
}

}  // namespace nickel::video
