#pragma once

#include "graphics/rhi/impl/render_pipeline.hpp"

namespace nickel::rhi::null {

class RenderPipelineImpl: public rhi::RenderPipelineImpl {
public:
    PipelineLayout GetLayout() const;
};

}