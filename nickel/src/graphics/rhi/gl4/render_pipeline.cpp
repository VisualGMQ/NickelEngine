#include "graphics/rhi/gl4/render_pipeline.hpp"
#include "graphics/rhi/gl4/glcall.hpp"
#include "graphics/rhi/gl4/shader.hpp"
#include "graphics/rhi/gl4/convert.hpp"

namespace nickel::rhi::gl4 {

RenderPipelineImpl::RenderPipelineImpl(const RenderPipeline::Descriptor& desc)
    : desc_{desc} {}

RenderPipelineImpl::~RenderPipelineImpl() {
    GL_CALL(glDeleteProgram(shaderId_));
}


void RenderPipelineImpl::createShader(const RenderPipeline::Descriptor& desc) {
    shaderId_ = glCreateProgram();

    auto vertexModule = static_cast<ShaderModuleImpl*>(desc.vertex.module.Impl());
    GLuint vertexId = vertexModule->CreateShader(GL_VERTEX_SHADER);
    GL_CALL(glAttachShader(shaderId_, vertexId));

    auto fragModule = static_cast<ShaderModuleImpl*>(desc.fragment.module.Impl());
    GLuint fragId = vertexModule->CreateShader(GL_VERTEX_SHADER);
    GL_CALL(glAttachShader(shaderId_, fragId));

    GL_CALL(glLinkProgram(shaderId_));

    int success;
    char infoLog[1024];
    GL_CALL(glGetProgramiv(shaderId_, GL_LINK_STATUS, &success));
    if (!success) {
        glGetProgramInfoLog(shaderId_, 1024, NULL, infoLog);
        LOGF(log_tag::GL, "shader link failed:\r\n", infoLog);
    }

    GL_CALL(glDeleteShader(vertexId));
    GL_CALL(glDeleteShader(fragId));
}

void RenderPipelineImpl::Apply() const {
    // TODO
}

}  // namespace nickel::rhi::gl4