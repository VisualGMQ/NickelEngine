#include "nickel/video/window.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::video {

class Window::Impl {
public:
    Impl(const std::string& title, int w, int h) {
        m_window = SDL_CreateWindow(title.c_str(), w, h, SDL_WINDOW_VULKAN);
        if (!m_window) {
            // TODO: not finish
        }
    }

private:
    SDL_Window* m_window;
};

Window::Window(const std::string& title, int w, int h)
    : m_impl{std::make_unique<Impl>(title, w, h)} {}

Window::Impl& Window::GetImpl() {
    return *m_impl;
}

const Window::Impl& Window::GetImpl() const {
    return *m_impl;
}

Window::~Window() {}

}  // namespace nickel::video