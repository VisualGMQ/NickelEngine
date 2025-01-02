#pragma once
#include "nickel/common/math/smatrix.hpp"
#include "nickel/internal/pch.hpp"
#include "nickel/video/window.hpp"

namespace nickel::video {

class Window::Impl {
public:
    Impl(const std::string& title, int w, int h);
    SDL_Window* GetWindow() const;
    SVector<uint32_t, 2> GetSize() const noexcept;

    SDL_Window* m_window;
};


}