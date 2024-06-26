#pragma once

#include "graphics/rhi/vk/pch.hpp"
#include "graphics/rhi/vk/util.hpp"
#include "graphics/rhi/impl/framebuffer.hpp"
#include "graphics/rhi/framebuffer.hpp"
#include "graphics/rhi/renderpass.hpp"

namespace nickel::rhi::vulkan {

class FramebufferImpl : public rhi::FramebufferImpl {
public:
    FramebufferImpl(vk::Device dev, const std::vector<TextureView>& views,
                    const Extent3D& extent, vk::RenderPass);
    ~FramebufferImpl();

    vk::Framebuffer fbo;

    auto& Views() const { return views_; }
    Extent3D Extent() const { return extent_; }

private:
    vk::Device dev_;
    std::vector<TextureView> views_;
    Extent3D extent_;
};

}  // namespace nickel::rhi::vulkan