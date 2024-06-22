#include "graphics/rhi/gl/shader.hpp"
#include "graphics/rhi/gl/convert.hpp"
#include "graphics/rhi/gl/glcall.hpp"

namespace nickel::rhi::gl {

ShaderModuleImpl::ShaderModuleImpl(const ShaderModule::Descriptor& desc)
    : desc_{desc} {}

GLuint ShaderModuleImpl::CreateShader(GLenum stage) {
    GLuint id = glCreateShader(stage);
    if (desc_.code.back() != '\0') {
        desc_.code.push_back('\0');
    }
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

}  // namespace nickel::rhi::gl