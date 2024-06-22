#pragma once

#include "graphics/rhi/impl/pipeline_layout.hpp"
#include "graphics/rhi/pipeline_layout.hpp"

namespace nickel::rhi::gl {

class PipelineLayoutImpl: public rhi::PipelineLayoutImpl {
public:
    explicit PipelineLayoutImpl(const PipelineLayout::Descriptor&);

    const PipelineLayout::Descriptor& Descriptor() const;

private:
    PipelineLayout::Descriptor desc_;
};

}