#include "graphics/rhi/gl4/pipeline_layout.hpp"

namespace nickel::rhi::gl4 {

PipelineLayoutImpl::PipelineLayoutImpl(const PipelineLayout::Descriptor& desc)
    : desc_{desc} {}

const PipelineLayout::Descriptor& PipelineLayoutImpl::Descriptor() const {
    return desc_;
}

}  // namespace nickel::rhi::gl4