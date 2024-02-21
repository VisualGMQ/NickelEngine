#pragma once

#include "graphics/rhi/impl/render_pipeline.hpp"
#include "graphics/rhi/render_pipeline.hpp"
#include "graphics/rhi/shader.hpp"
#include "graphics/rhi/gl4/glpch.hpp"

namespace nickel::rhi::gl4 {

class RenderPipelineImpl: public rhi::RenderPipelineImpl {
public:
    explicit RenderPipelineImpl(const RenderPipeline::Descriptor&);
    ~RenderPipelineImpl();

    void Apply() const;

    auto& Descriptor() const { return desc_; }

    PipelineLayout GetLayout() const override { return {}; }

private:
    RenderPipeline::Descriptor desc_;
    GLuint shaderId_;

    void createShader(const RenderPipeline::Descriptor&);
};

}