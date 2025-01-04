#include "nickel/graphics/internal/framebuffer_impl.hpp"

#include "nickel/graphics/image_view.hpp"
#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/graphics/internal/image_view_impl.hpp"
#include "nickel/graphics/internal/render_pass_impl.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

FramebufferImpl::FramebufferImpl(DeviceImpl& dev,
                                 const Framebuffer::Descriptor& desc)
    : m_device{dev} {
    std::vector<VkImageView> attachments;
    attachments.reserve(desc.m_views.size());

    for (auto& view : desc.m_views) {
        attachments.push_back(view.Impl().m_view);
    }

    VkFramebufferCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    ci.height = desc.m_extent.h;
    ci.width = desc.m_extent.w;
    ci.layers = desc.m_extent.l;
    ci.attachmentCount = attachments.size();
    ci.pAttachments = attachments.data();
    ci.renderPass = desc.m_render_pass.Impl().m_render_pass;

    VK_CALL(vkCreateFramebuffer(dev.m_device, &ci, nullptr, &m_fbo));

    m_views = desc.m_views;
}

FramebufferImpl::~FramebufferImpl() {
    vkDestroyFramebuffer(m_device.m_device, m_fbo, nullptr);
}

}  // namespace nickel::graphics