#pragma once

#include "graphics/rhi/impl/pipeline_layout.hpp"
#include "graphics/rhi/pipeline_layout.hpp"

namespace nickel::rhi::gl4 {

class PipelineLayoutImpl: public rhi::PipelineLayoutImpl {
public:
    explicit PipelineLayoutImpl(const PipelineLayout::Descriptor&);
};

}