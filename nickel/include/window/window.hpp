#pragma once

#include "pch.hpp"
#include "config/config.hpp"
#include "core/log.hpp"
#include "core/cgmath.hpp"
#include "core/errort.hpp"
#include "core/gogl.hpp"

namespace nickel {

class Window final {
public:
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&& o) {
        window_ = o.window_;
        o.window_ = nullptr;
    }

    Window& operator=(Window&& o) {
        if (&o != this) {
            window_ = o.window_;
            o.window_ = nullptr;
        }
        return *this;
    }

    Window(const std::string& title, int width, int height);
    ~Window();

    bool ShouldClose() const;
    void SwapBuffer() const;

private:
    GLFWwindow* window_;
};

class WindowBuilder final {
public:
    auto& Title(const std::string& title) {
        title_ = title;
        return *this;
    }

    auto& Size(const cgmath::Vec2& size) {
        size_ = size;
        return *this;
    }

    static WindowBuilder FromConfig(const std::string& filename);
    static WindowBuilder Default();

    Window Build() {
        return Window(title_, static_cast<int>(size_.x), static_cast<int>(size_.y));
    }

private:
    std::string title_;
    cgmath::Vec2 size_;
};

}