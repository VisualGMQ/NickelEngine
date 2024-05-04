#pragma once

#include "graphics/rhi/impl/renderpass.hpp"
#include "graphics/rhi/renderpass.hpp"

namespace nickel::rhi::gl4 {

class RenderPassImpl : public rhi::RenderPassImpl {
public:
    RenderPassImpl(const RenderPass::Descriptor& desc) : desc_{desc} {}

    const RenderPass::Descriptor& GetDescriptor() const override {
        return desc_;
    }

private:
    RenderPass::Descriptor desc_;
};

}  // namespace nickel::rhi::gl4