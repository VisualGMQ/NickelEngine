#pragma once

#include "graphics/rhi/command.hpp"
#include "graphics/rhi/impl/renderpass.hpp"
#include "graphics/rhi/vk/pch.hpp"
#include "graphics/rhi/vk/util.hpp"
#include "graphics/rhi/renderpass.hpp"


namespace nickel::rhi::vulkan {

class DeviceImpl;

class RenderPassImpl : public rhi::RenderPassImpl {
public:
    RenderPassImpl(DeviceImpl&, const RenderPass::Descriptor& desc);
    ~RenderPassImpl();

    vk::RenderPass renderPass;

private:
    vk::Device dev_;
    RenderPass::Descriptor desc_;

    void createRenderPass(DeviceImpl&, const RenderPass::Descriptor& desc);
};

}  // namespace nickel::rhi::vulkan