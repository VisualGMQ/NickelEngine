#include "nickel/video/internal/window_impl.hpp"

#include "nickel/common/log.hpp"

namespace nickel::video {

Window::Impl::Impl(const std::string& title, int w, int h) {
#ifdef NICKEL_PLATFORM_ANDROID
    m_window = SDL_CreateWindow(title.c_str(), 0, 0, SDL_WINDOW_VULKAN);
#else
    m_window = SDL_CreateWindow(title.c_str(), w, h,
                                SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
#endif
    if (!m_window) {
        LOGC("create window failed");
    }
}

Window::Impl::Impl(void* handle) {
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_VULKAN_BOOLEAN, true);
    SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER, handle);
    
    m_window = SDL_CreateWindowWithProperties(props);
    if (!m_window) {
        LOGC("create window failed");
    }
    SDL_DestroyProperties(props);
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

void Window::Impl::SetTitle(const std::string& title) const {
    SDL_SetWindowTitle(m_window, title.c_str());
}

}  // namespace nickel::video
