#pragma once

#include "config/config.hpp"
#include "core/cgmath.hpp"
#include "core/gogl.hpp"
#include "core/log.hpp"
#include "core/log_tag.hpp"
#include "pch.hpp"

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
    void Resize(int w, int h);
    void SetTitle(const std::string& title);

    cgmath::Vec2 Size() const;

    void* Raw() { return window_; }

private:
    GLFWwindow* window_;
};

class WindowBuilder final {
public:
    struct Data {
        std::string title;
        cgmath::Vec2 size;

        static Data Default();
    };

    WindowBuilder(const Data& data) : buildData_(data) {}

    static WindowBuilder FromConfig(std::string_view filename);
    static WindowBuilder Default();

    Window Build() {
        return Window(buildData_.title, static_cast<int>(buildData_.size.x),
                      static_cast<int>(buildData_.size.y));
    }

private:
    Data buildData_;
};

}  // namespace nickel