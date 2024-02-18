#include "graphics/rhi/gl4/shader.hpp"
#include "graphics/rhi/gl4/convert.hpp"
#include "graphics/rhi/gl4/glcall.hpp"

namespace nickel::rhi::gl4 {

ShaderModuleImpl::ShaderModuleImpl(const ShaderModule::Descriptor& desc)
    : desc_{desc} {}

GLuint ShaderModuleImpl::CreateShader(GLenum stage) {
    GLuint id = glCreateShader(stage);
    const char* source = desc_.code.data();
    GL_CALL(glShaderSource(id, 1, &source, nullptr));
    GL_CALL(glCompileShader(id));

    int success;
    char infoLog[1024];
    GL_CALL(glGetShaderiv(id, GL_COMPILE_STATUS, &success));
    if (!success) {
        GL_CALL(glGetShaderInfoLog(id, 1024, NULL, infoLog));
        LOGF(log_tag::GL, " shader compile failed:\r\n", infoLog);
    }
    return id;
}

}  // namespace nickel::rhi::gl4