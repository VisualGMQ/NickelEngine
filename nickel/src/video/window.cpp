#include "video/window.hpp"
#include "config/config.hpp"
#include "common/log_tag.hpp"
#include "common/profile.hpp"
#include "video/event.hpp"

#include "SDL.h"

namespace nickel {

Window::Window(const std::string& title, int width, int height, bool supportVulkan)
    : title_(title) {
    window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, width, height,
        (supportVulkan ? SDL_WINDOW_VULKAN : SDL_WINDOW_OPENGL) |
            SDL_WINDOW_SHOWN);

    if (!window_) {
        LOGE(log_tag::SDL2, "create window failed");
    }
}

cgmath::Vec2 Window::Size() const {
    int w, h;
    SDL_GetWindowSize(window_, &w, &h);
    return {static_cast<float>(w), static_cast<float>(h)};
}

void Window::Resize(int w, int h) {
    SDL_SetWindowSize(window_, w, h);
}

void Window::SetTitle(const std::string& title) {
    SDL_SetWindowTitle(window_, title.c_str());
    title_ = title;
}

void Window::SetFullScreen(bool b) {
    if (b) {
        SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(window_, 0);
    }
}

void Window::SetResizable(bool b) {
    SDL_SetWindowResizable(window_, static_cast<SDL_bool>(b));
}

bool Window::IsMinimized() const {
    auto flags = SDL_GetWindowFlags(window_);
    return flags & SDL_WINDOW_MINIMIZED;
}

bool Window::IsMaximized() const {
    auto flags = SDL_GetWindowFlags(window_);
    return flags & SDL_WINDOW_MAXIMIZED;
}

Window::~Window() {
    SDL_DestroyWindow(window_);
}

WindowBuilder::Data WindowBuilder::Data::Default() {
    return {
        std::string{config::DefaultWindowTitle},
        {config::DefaultWindowWidth, config::DefaultWindowHeight}
    };
}

WindowBuilder WindowBuilder::Default() {
    return WindowBuilder(WindowBuilder::Data::Default());
}

cgmath::Vec2 Screen::Size() {
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(0, &mode);
        return cgmath::Vec2(mode.w, mode.h);
    }

}  // namespace nickel