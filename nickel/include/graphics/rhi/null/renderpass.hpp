#pragma once

#include "graphics/rhi/impl/renderpass.hpp"

namespace nickel::rhi::null {

class RenderPassImpl : public rhi::RenderPassImpl {
    const RenderPass::Descriptor& GetDescriptor() const { return desc_; }

private:
    RenderPass::Descriptor desc_;
};

}  // namespace nickel::rhi::null
