#pragma once

#include "graphics/rhi/impl/renderpass.hpp"
#include "graphics/rhi/renderpass.hpp"

namespace nickel::rhi::gl4 {

class RenderPassImpl: public rhi::RenderPassImpl {
public:
    explicit RenderPassImpl(const RenderPass::Descriptor&);

    const RenderPass::Descriptor& Descriptor() const;

private:
    RenderPass::Descriptor desc_;
};

}