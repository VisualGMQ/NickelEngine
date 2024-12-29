#pragma once
#include "nickel/video/window.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::video {

class Window::Impl {
public:
    Impl(const std::string& title, int w, int h);
    SDL_Window* GetWindow() const;

    SDL_Window* m_window;
};


}