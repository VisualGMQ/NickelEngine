#include "video/window.hpp"
#include "config/config.hpp"
#include "common/log_tag.hpp"
#include "common/profile.hpp"
#include "graphics/renderer2d.hpp"
#include "video/event.hpp"

#include "SDL.h"

namespace nickel {

Window::Window(const std::string& title, int width, int height)
    : title_(title) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, config::GLMajorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, config::GLMinorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);  // 核心库

    window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, width, height,
                               SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

    if (!window_) {
        LOGE(log_tag::SDL2, "create window failed");
    } else {
        // SDL_GL_CreateContext(window_);

        // if (gladLoadGL() == 0) {
        //     LOGE("GLAD", "load opengl ", config::GLMajorVersion, ".",
        //          config::GLMinorVersion, " failed");
        // }
        // GL_CALL(glViewport(0, 0, width, height));
    }
}

void Window::SwapBuffer() const {
    // SDL_GL_SwapWindow(window_);
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