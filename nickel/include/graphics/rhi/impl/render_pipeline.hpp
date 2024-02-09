#pragma once

#include "graphics/rhi/pipeline_layout.hpp"
namespace nickel::rhi {

class RenderPipelineImpl {
public:
    virtual ~RenderPipelineImpl() = default;

    virtual PipelineLayout GetLayout() const = 0;
};

}