#pragma once

#include "config/config.hpp"
#include "common/cgmath.hpp"
#include "common/log.hpp"
#include "common/log_tag.hpp"
#include "common/singlton.hpp"

typedef struct SDL_Window SDL_Window;

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

    Window(const std::string& title, int width, int height, bool supportVulkan = false);
    ~Window();

    void Resize(int w, int h);
    void SetTitle(const std::string& title);

    void SetFullScreen(bool b);
    void SetResizable(bool b);

    bool IsMinimized() const;
    bool IsMaximized() const;

    std::string_view Title() const { return title_; }

    cgmath::Vec2 Size() const;

    void* Raw() const { return window_; }

    bool ShouldClose() const { return shouldClose_; }

    /**
     * @brief close window
     */
    void Close() { shouldClose_ = true; }

private:
    SDL_Window* window_;
    std::string title_;
    bool shouldClose_ = false;
};

class WindowBuilder final {
public:
    struct Data {
        std::string title;
        cgmath::Vec2 size;

        static Data Default();
    };

    WindowBuilder(const Data& data) : buildData_(data) {}

    static WindowBuilder Default();

    Window Build(bool supportVulkan) {
        return Window(buildData_.title, static_cast<int>(buildData_.size.x),
                      static_cast<int>(buildData_.size.y), supportVulkan);
    }

private:
    Data buildData_;
};

class Screen final: public Singlton<Screen, false> {
public:
    cgmath::Vec2 Size();
};

}  // namespace nickel