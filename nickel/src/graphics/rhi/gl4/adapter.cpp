#include "graphics/rhi/gl4/adapter.hpp"
#include "common/log.hpp"
#include "common/log_tag.hpp"
#include "graphics/rhi/gl4/device.hpp"
#include "graphics/rhi/gl4/glcall.hpp"
#include "graphics/rhi/gl4/glpch.hpp"

namespace nickel::rhi::gl4 {

#ifdef NICKEL_HAS_GL4
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id,
                            GLenum severity, GLsizei length,
                            const char *message, const void *userParam) {
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            std::cout << "Source: API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            std::cout << "Source: Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            std::cout << "Source: Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            std::cout << "Source: Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            std::cout << "Source: Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            std::cout << "Source: Other";
            break;
    }
    std::cout << "; " << std::endl;

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            std::cout << "Type: Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            std::cout << "Type: Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            std::cout << "Type: Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            std::cout << "Type: Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            std::cout << "Type: Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            std::cout << "Type: Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            std::cout << "Type: Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            std::cout << "Type: Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            std::cout << "Type: Other";
            break;
    }
    std::cout << "; " << std::endl;

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            std::cout << "Severity: high";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::cout << "Severity: medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            std::cout << "Severity: low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            std::cout << "Severity: notification";
            break;
    }
    std::cout << message << std::endl;
}
#endif

AdapterImpl::AdapterImpl(SDL_Window *window) : window{window} {
#ifdef NICKEL_HAS_GL4
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#endif

    ctx_ = SDL_GL_CreateContext(window);
    if (!ctx_) {
        LOGE(log_tag::GL, "create opengl context failed");
        exit(1);
    }

#ifdef NICKEL_HAS_GL4
    gladLoadGL();

    int flags;
    GL_CALL(glGetIntegerv(GL_CONTEXT_FLAGS, &flags));
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        LOGI(log_tag::GL, "enable opengl debug output");
        GL_CALL(glEnable(GL_DEBUG_OUTPUT));
        GL_CALL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
        GL_CALL(glDebugMessageCallback(glDebugOutput, nullptr));
        GL_CALL(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
                                      0, nullptr, GL_TRUE));
    }
#endif

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    GL_CALL(glViewport(0, 0, w, h));

    querySupportLimits();
}

AdapterImpl::~AdapterImpl() {
    SDL_GL_DeleteContext(ctx_);
}

GPUSupportFeatures AdapterImpl::Features() {
    return {};
}

const GPUSupportLimits &AdapterImpl::Limits() const {
    return limits_;
}

Device AdapterImpl::RequestDevice() {
    return Device{*this};
}

Adapter::Info AdapterImpl::RequestAdapterInfo() const {
    Adapter::Info info;
    info.api = APIPreference::GL;
    info.vendorID = 0;
    info.device = (const char *)glGetString(GL_RENDERER);
    return info;
}

void AdapterImpl::querySupportLimits() {
    GLint value;
    GL_CALL(glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &value));
    limits_.minUniformBufferOffsetAlignment = value;
    limits_.maxPushConstantSize = _NICKEL_GL_MAX_PUSHCONSTANT_SIZE;
    limits_.supportSeparateSampler = false;

#ifdef NICKEL_HAS_GL4
    GL_CALL(glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &value));
    limits_.minStorageBufferOffsetAlignment = value;
    limits_.supportGeometryShader = true;
    limits_.supportGeometryShader = true;
    limits_.supportPolygonModeChange = true;
#else
    limits_.supportGeometryShader = false;
    limits_.supportGeometryShader = false;
    limits_.supportPolygonModeChange = false;
#endif
}

}  // namespace nickel::rhi::gl4