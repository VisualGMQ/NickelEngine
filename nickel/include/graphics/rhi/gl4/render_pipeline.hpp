#pragma once

#include "graphics/rhi/impl/render_pipeline.hpp"
#include "graphics/rhi/render_pipeline.hpp"
#include "graphics/rhi/shader.hpp"
#include "graphics/rhi/gl4/glpch.hpp"

namespace nickel::rhi::gl4 {

GLuint BindVertexLayout2VAO(GLuint vao,
                            const RenderPipeline::VertexState& state);

class RenderPipelineImpl: public rhi::RenderPipelineImpl {
public:
    explicit RenderPipelineImpl(const RenderPipeline::Descriptor&);
    ~RenderPipelineImpl();

    void Apply() const;

    auto& Descriptor() const { return desc_; }

    PipelineLayout GetLayout() const override { return {}; }

    GLuint GetDefaultVAO() const { return vao_; }

private:
    RenderPipeline::Descriptor desc_;
    GLuint shaderId_;
    GLuint vao_;

    void createShader(const RenderPipeline::Descriptor&);
    void createVertexArray(const RenderPipeline::VertexState&);
};

}