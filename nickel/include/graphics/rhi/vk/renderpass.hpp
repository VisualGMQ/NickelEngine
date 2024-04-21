#pragma once

#include "graphics/rhi/command.hpp"
#include "graphics/rhi/impl/renderpass.hpp"
#include "graphics/rhi/vk/pch.hpp"
#include "graphics/rhi/vk/util.hpp"
#include "graphics/rhi/renderpass.hpp"


namespace nickel::rhi::vulkan {

inline vk::ImageLayout GetDepthStencilLayoutAfterSubpass(const RenderPass::Descriptor& desc) {
    return GetLayoutByFormat(desc.depthStencilAttachment->view.Format(),
                             desc.depthStencilAttachment->stencilReadOnly,
                             desc.depthStencilAttachment->depthReadOnly);
}

inline vk::ImageLayout GetImageLayoutAfterSubpass(TextureFormat viewFmt) {
    return GetLayoutByFormat(viewFmt);
}

class DeviceImpl;

class RenderPassImpl : public rhi::RenderPassImpl {
public:
    RenderPassImpl(DeviceImpl&, const RenderPass::Descriptor& desc);
    ~RenderPassImpl();

    vk::RenderPass renderPass;
    RenderPass::Descriptor desc;

    const RenderPass::Descriptor& GetDescriptor() const override;

private:
    vk::Device dev_;

    void createRenderPass(DeviceImpl&, const RenderPass::Descriptor& desc);
};

}  // namespace nickel::rhi::vulkan