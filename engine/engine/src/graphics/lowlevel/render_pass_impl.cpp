#include "nickel/graphics/lowlevel/internal/render_pass_impl.hpp"

#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/enum_convert.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"

namespace nickel::graphics {

RenderPassImpl::RenderPassImpl(DeviceImpl& device,
                               const RenderPass::Descriptor& descriptor)
    : m_dev{device} {
    VkRenderPassCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    std::vector<VkAttachmentDescription> attachments;
    attachments.reserve(descriptor.m_attachments.size());
    for (auto& attachment : descriptor.m_attachments) {
        VkAttachmentDescription desc{};
        desc.format = Format2Vk(attachment.m_format);
        desc.samples = static_cast<VkSampleCountFlagBits>(SampleCount2Vk(attachment.m_samples));
        desc.finalLayout = ImageLayout2Vk(attachment.m_final_layout);
        desc.initialLayout = ImageLayout2Vk(attachment.m_initial_layout);
        desc.loadOp = AttachmentLoadOp2Vk(attachment.m_load_op);
        desc.storeOp = AttachmentStoreOp2Vk(attachment.m_store_op);
        desc.stencilLoadOp = AttachmentLoadOp2Vk(attachment.m_stencil_load_op);
        desc.stencilStoreOp = AttachmentStoreOp2Vk(attachment.m_store_op);
        attachments.emplace_back(desc);
    }

    std::vector<VkSubpassDependency> dependencies;
    dependencies.reserve(descriptor.m_dependencies.size());
    for (auto& dependency : descriptor.m_dependencies) {
        VkSubpassDependency dep{};
        dep.dstSubpass = dependency.m_dst_subpass;
        dep.srcSubpass = dependency.m_src_subpass;
        dep.dstAccessMask = Access2Vk(dependency.m_dst_access_mask);
        dep.dstStageMask = PipelineStage2Vk(dependency.m_dst_stage_mask);
        dep.srcAccessMask = Access2Vk(dependency.m_src_access_mask);
        dep.srcStageMask = PipelineStage2Vk(dependency.m_src_stage_mask);
        dependencies.emplace_back(dep);
    }

    std::vector<VkSubpassDescription> subpasses;
    subpasses.reserve(descriptor.m_subpasses.size());

    size_t color_attachments_count = 0;
    size_t input_attachments_count = 0;
    size_t resolve_attachments_count = 0;
    size_t preserve_attachments_count = 0;
    for (auto& subpass : descriptor.m_subpasses) {
        color_attachments_count += subpass.m_color_attachments.size();
        input_attachments_count += subpass.m_input_attachments.size();
        resolve_attachments_count += subpass.m_resolve_attachments.size();
        preserve_attachments_count += subpass.m_preserve_attachments.size();
    }
    
    std::vector<VkAttachmentReference> color_attachments;
    std::vector<VkAttachmentReference> input_attachments;
    std::vector<VkAttachmentReference> resolve_attachments;
    std::vector<uint32_t> preserve_attachments;
    color_attachments.reserve(color_attachments_count);
    input_attachments.reserve(input_attachments_count);
    resolve_attachments.reserve(resolve_attachments_count);
    preserve_attachments.reserve(preserve_attachments_count);

    for (auto& subpass : descriptor.m_subpasses) {
        VkSubpassDescription sub{};

        sub.pColorAttachments =
            color_attachments.data() + color_attachments.size();
        for (auto& attachment : subpass.m_color_attachments) {
            VkAttachmentReference att{};
            att.attachment = attachment.m_attachment;
            att.layout = ImageLayout2Vk(attachment.m_layout);
            color_attachments.emplace_back(att);
        }
        sub.colorAttachmentCount = subpass.m_color_attachments.size();

        sub.pInputAttachments =
            input_attachments.data() + input_attachments.size();
        for (auto& attachment : subpass.m_input_attachments) {
            VkAttachmentReference att{};
            att.attachment = attachment.m_attachment;
            att.layout = ImageLayout2Vk(attachment.m_layout);
            input_attachments.emplace_back(att);
        }
        sub.inputAttachmentCount = subpass.m_input_attachments.size();

        sub.pResolveAttachments =
            resolve_attachments.data() + resolve_attachments.size();
        for (auto& attachment : subpass.m_resolve_attachments) {
            VkAttachmentReference att{};
            att.attachment = attachment.m_attachment;
            att.layout = ImageLayout2Vk(attachment.m_layout);
            resolve_attachments.emplace_back(att);
        }
        sub.preserveAttachmentCount = subpass.m_preserve_attachments.size();

        VkAttachmentReference depthStencilAttachment{};
        if (subpass.m_depth_stencil_attachment) {
            depthStencilAttachment.attachment =
                subpass.m_depth_stencil_attachment->m_attachment;
            depthStencilAttachment.layout =
                ImageLayout2Vk(subpass.m_depth_stencil_attachment->m_layout);
            sub.pDepthStencilAttachment = &depthStencilAttachment;
        }

        sub.pPreserveAttachments =
            preserve_attachments.data() + preserve_attachments.size();
        for (auto& attachment : subpass.m_preserve_attachments) {
            preserve_attachments.emplace_back(attachment);
        }
        sub.preserveAttachmentCount = preserve_attachments.size();

        subpasses.emplace_back(sub);
    }

    ci.attachmentCount = attachments.size();
    ci.pAttachments = attachments.data();

    ci.dependencyCount = dependencies.size();
    ci.pDependencies = dependencies.data();

    ci.subpassCount = subpasses.size();
    ci.pSubpasses = subpasses.data();

    ci.subpassCount = subpasses.size();
    ci.pSubpasses = subpasses.data();

    VK_CALL(vkCreateRenderPass(device.m_device, &ci, nullptr,
                               &m_render_pass));
}

RenderPassImpl::~RenderPassImpl() {
    vkDestroyRenderPass(m_dev.m_device, m_render_pass, nullptr);
}

void RenderPassImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_dev.m_render_pass_allocator.MarkAsGarbage(this);
    }
}

}  // namespace nickel::graphics