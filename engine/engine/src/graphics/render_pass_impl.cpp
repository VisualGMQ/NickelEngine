#include "nickel/graphics/internal/render_pass_impl.hpp"

#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

RenderPassImpl::RenderPassImpl(DeviceImpl& device,
                               const RenderPass::Descriptor& descriptor)
    : m_dev{device} {
    VkRenderPassCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = descriptor.attachments.size();
    createInfo.pAttachments = descriptor.attachments.data();
    createInfo.subpassCount = descriptor.subpasses.size();
    createInfo.pSubpasses = descriptor.subpasses.data();
    createInfo.dependencyCount = descriptor.dependencies.size();
    createInfo.pDependencies = descriptor.dependencies.data();

    VK_CALL(vkCreateRenderPass(device.m_device, &createInfo, nullptr,
                               &m_render_pass));
}

RenderPassImpl::~RenderPassImpl() {
    vkDestroyRenderPass(m_dev.m_device, m_render_pass, nullptr);
}

}  // namespace nickel::graphics