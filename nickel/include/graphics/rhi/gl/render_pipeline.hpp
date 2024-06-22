#pragma once

#include "graphics/rhi/impl/render_pipeline.hpp"
#include "graphics/rhi/render_pipeline.hpp"
#include "graphics/rhi/shader.hpp"
#include "graphics/rhi/gl/glpch.hpp"

namespace nickel::rhi::gl {

class DeviceImpl;

class RenderPipelineImpl: public rhi::RenderPipelineImpl {
public:
    explicit RenderPipelineImpl(DeviceImpl& dev, const RenderPipeline::Descriptor&);
    ~RenderPipelineImpl();

    void Apply() const;

    auto& Descriptor() const { return desc_; }

    PipelineLayout GetLayout() const override { return desc_.layout; }

    GLuint GetDefaultVAO() const { return vao_; }
    GLuint GetShaderID() const { return shaderId_; }

private:
    DeviceImpl& dev_;
    RenderPipeline::Descriptor desc_;
    GLuint shaderId_ = 0;
    GLuint vao_;

    void createShader(const RenderPipeline::Descriptor&);
    void createVertexArray(const RenderPipeline::VertexState&);
};

}