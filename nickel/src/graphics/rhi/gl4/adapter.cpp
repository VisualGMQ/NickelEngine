#include "graphics/rhi/gl4/adapter.hpp"
#include "glad/glad.h"
#include "common/log_tag.hpp"
#include "common/log.hpp"
#include "graphics/rhi/gl4/glcall.hpp"

namespace nickel::rhi::gl4 {

AdapterImpl::AdapterImpl(SDL_Window* window) {
    ctx_ = SDL_GL_CreateContext(window);

    if (gladLoadGL() == 0) {
        LOGE("GLAD", "load opengl ", 4, ".", 5, " failed");
    }
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
}

Adapter::Info AdapterImpl::RequestAdapterInfo() {
    Adapter::Info info;
    info.api = APIPreference::GL;
    info.vendorID = 0;
    info.device = (const char*)glGetString(GL_RENDERER);
    return info;
}

}  // namespace nickel::rhi::gl4