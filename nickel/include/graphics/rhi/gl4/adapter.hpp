#pragma once

#include "SDL.h"
#include "graphics/rhi/impl/adapter.hpp"


namespace nickel::rhi::gl4 {

class AdapterImpl : public rhi::AdapterImpl {
public:
    AdapterImpl(SDL_Window* window);
    ~AdapterImpl();

    GPUSupportFeatures Features() override;
    GPUSupportLimits Limits() override;
    Device RequestDevice() override;
    Adapter::Info RequestAdapterInfo() override;

    SDL_Window* window;

private:
    SDL_GLContext ctx_;
};

}  // namespace nickel::rhi::gl4