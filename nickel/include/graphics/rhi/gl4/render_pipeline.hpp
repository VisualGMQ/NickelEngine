#pragma once

#include "graphics/rhi/impl/render_pipeline.hpp"
#include "graphics/rhi/render_pipeline.hpp"
#include "graphics/rhi/shader.hpp"
#include "glad/glad.h"

namespace nickel::rhi::gl4 {

class RenderPipelineImpl: public rhi::RenderPipelineImpl {
public:
    explicit RenderPipelineImpl(const RenderPipeline::Descriptor&);
    ~RenderPipelineImpl();

    void Apply() const;

private:
    RenderPipeline::Descriptor desc_;
    GLuint shaderId_;

    void createShader(const RenderPipeline::Descriptor&);
};

}