#pragma once

#include "graphics/rhi/renderpass.hpp"

namespace nickel::rhi {

class RenderPassImpl {
public:
    virtual ~RenderPassImpl() = default;
    virtual const RenderPass::Descriptor& GetDescriptor() const = 0;
};

}