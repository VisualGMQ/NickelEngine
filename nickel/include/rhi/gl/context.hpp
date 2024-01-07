#pragma once

#include "rhi/context.hpp"
#include "rhi/gl/instance.hpp"
#include "rhi/gl/device.hpp"

namespace nickel::rhi::gl {

inline void SetAPIHint() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
                        config::GLMajorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
                        config::GLMinorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
}

class Context final: public rhi::Context {
public:
    explicit Context(Window& window)
        : instance_{std::make_unique<gl::Instance>(window)},
          device_{std::make_unique<gl::Device>()} {}

    const rhi::Instance& Instance() const override {
        return *instance_;
    }

    const rhi::Device& Device() const override {
        return *device_;
    }

private:
    std::unique_ptr<gl::Instance> instance_;
    std::unique_ptr<gl::Device> device_;
};

}