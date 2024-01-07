#include "rhi/gl/shader.hpp"

namespace nickel::rhi::gl {

GLenum ShaderModuleType2GL(rhi::ShaderModule::Type type) {
    switch (type) {
        case ShaderModule::Type::Vertex:
            return GL_VERTEX_SHADER;
        case ShaderModule::Type::Fragment:
            return GL_FRAGMENT_SHADER;
        case ShaderModule::Type::Compute:
            return GL_COMPUTE_SHADER;
        default:
            Assert(false, "invalid shader module type");
            return -1;
    }
}

ShaderModule::ShaderModule(rhi::ShaderModule::Type type, const std::vector<uint8_t>& code)
    : rhi::ShaderModule{type} {
    id_ = glCreateShader(ShaderModuleType2GL(type));
    const char* source = (const char*)code.data();
    GL_CALL(glShaderSource(id_, 1, &source, nullptr));
    GL_CALL(glCompileShader(id_));

    int success;
    char infoLog[1024];
    GL_CALL(glGetShaderiv(id_, GL_COMPILE_STATUS, &success));
    if (!success) {
        GL_CALL(glGetShaderInfoLog(id_, 1024, NULL, infoLog));
        LOGF(log_tag::GL, ShaderModuleType2GL(type), " shader compile failed:\r\n",
             infoLog);
    }
}

}  // namespace nickel::rhi::gl