#pragma once

#include "rhi/instance.hpp"

namespace nickel::rhi::gl {

class Instance: public rhi::Instance {
public:
    explicit Instance(Window& window) {
        SDL_GL_CreateContext((SDL_Window*)window.Raw());
    }

    Instance(const Instance&) = delete;
    Instance& operator=(const Instance&) = delete;

    Instance(Instance&& o) { swap(o, *this); }

    Instance& operator=(Instance&& o) {
        if (this != &o) {
            swap(o, *this);
        }
        return *this;
    }

    ~Instance() { SDL_GL_DeleteContext(ctx_); }

private:
    SDL_GLContext ctx_;

    friend void swap(Instance& o1, Instance& o2) noexcept {
        using std::swap;
        swap(o1.ctx_, o2.ctx_);
    }
};

}  // namespace nickel::rhi::gl