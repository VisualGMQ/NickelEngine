#include "graphics/rhi/gl4/adapter.hpp"
#include "graphics/rhi/gl4/glpch.hpp"
#include "graphics/rhi/gl4/device.hpp"
#include "common/log_tag.hpp"
#include "common/log.hpp"
#include "graphics/rhi/gl4/glcall.hpp"

namespace nickel::rhi::gl4 {

AdapterImpl::AdapterImpl(SDL_Window* window): window{window} {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    ctx_ = SDL_GL_CreateContext(window);

    gladLoadGL();

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    GL_CALL(glViewport(0, 0, w, h));
}

AdapterImpl::~AdapterImpl() {
    SDL_GL_DeleteContext(ctx_);
}

GPUSupportFeatures AdapterImpl::Features() {
    return {};
}

GPUSupportLimits AdapterImpl::Limits() {
    return {};
}

Device AdapterImpl::RequestDevice() {
    return Device{*this};
}

Adapter::Info AdapterImpl::RequestAdapterInfo() {
    Adapter::Info info;
    info.api = APIPreference::GL;
    info.vendorID = 0;
    info.device = (const char*)glGetString(GL_RENDERER);
    return info;
}

}  // namespace nickel::rhi::gl4