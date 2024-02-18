#include "graphics/rhi/gl4/renderpass.hpp"

namespace nickel::rhi::gl4 {

RenderPassImpl::RenderPassImpl(const RenderPass::Descriptor& desc)
    : desc_{desc} {}

const RenderPass::Descriptor& RenderPassImpl::Descriptor() const {
    return desc_;
}

}  // namespace nickel::rhi::gl4