#pragma once

#include "SDL.h"
#include "graphics/rhi/impl/adapter.hpp"


namespace nickel::rhi::gl4 {

class AdapterImpl : public rhi::AdapterImpl {
public:
    AdapterImpl(SDL_Window* window);
    ~AdapterImpl();

    GPUSupportFeatures Features() override;
    const GPUSupportLimits& Limits() const override;
    Device RequestDevice() override;
    Adapter::Info RequestAdapterInfo() override;

    SDL_Window* window;

private:
    SDL_GLContext ctx_;
    GPUSupportLimits limits_;

    void querySupportLimits();
};

}  // namespace nickel::rhi::gl4