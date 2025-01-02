#include "nickel/video/window.hpp"
#include "nickel/internal/pch.hpp"
#include "nickel/video/internal/window_impl.hpp"

namespace nickel::video {

Window::Window(const std::string& title, int w, int h)
    : m_impl{std::make_unique<Impl>(title, w, h)} {}

Window::Impl& Window::GetImpl() {
    return *m_impl;
}

const Window::Impl& Window::GetImpl() const {
    return *m_impl;
}

SVector<uint32_t, 2> Window::GetSize() const noexcept {
    return m_impl->GetSize();
}

Window::~Window() {}

}  // namespace nickel::video