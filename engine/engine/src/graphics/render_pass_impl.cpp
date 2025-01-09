#include "nickel/graphics/internal/render_pass_impl.hpp"

#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/graphics/internal/enum_convert.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

RenderPassImpl::RenderPassImpl(DeviceImpl& device,
                               const RenderPass::Descriptor& descriptor)
    : m_dev{device} {
    VkRenderPassCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    std::vector<VkAttachmentDescription> attachments;
    attachments.reserve(descriptor.attachments.size());
    for (auto& attachment : descriptor.attachments) {
        VkAttachmentDescription desc{};
        desc.format = Format2Vk(attachment.format);
        desc.samples = static_cast<VkSampleCountFlagBits>(SampleCount2Vk(attachment.samples));
        desc.finalLayout = ImageLayout2Vk(attachment.finalLayout);
        desc.initialLayout = ImageLayout2Vk(attachment.initialLayout);
        desc.loadOp = AttachmentLoadOp2Vk(attachment.loadOp);
        desc.storeOp = AttachmentStoreOp2Vk(attachment.storeOp);
        desc.stencilLoadOp = AttachmentLoadOp2Vk(attachment.stencilLoadOp);
        desc.stencilStoreOp = AttachmentStoreOp2Vk(attachment.storeOp);
        attachments.emplace_back(desc);
    }

    std::vector<VkSubpassDependency> dependencies;
    dependencies.reserve(descriptor.dependencies.size());
    for (auto& dependency : descriptor.dependencies) {
        VkSubpassDependency dep{};
        dep.dstSubpass = dependency.dstSubpass;
        dep.srcSubpass = dependency.srcSubpass;
        dep.dstAccessMask = Access2Vk(dependency.dstAccessMask);
        dep.dstStageMask = PipelineStage2Vk(dependency.dstStageMask);
        dep.srcAccessMask = Access2Vk(dependency.srcAccessMask);
        dep.srcStageMask = PipelineStage2Vk(dependency.srcStageMask);
        dependencies.emplace_back(dep);
    }

    std::vector<VkSubpassDescription> subpasses;
    subpasses.reserve(descriptor.subpasses.size());

    size_t color_attachments_count = 0;
    size_t input_attachments_count = 0;
    size_t resolve_attachments_count = 0;
    size_t preserve_attachments_count = 0;
    for (auto& subpass : descriptor.subpasses) {
        color_attachments_count += subpass.colorAttachments.size();
        input_attachments_count += subpass.inputAttachments.size();
        resolve_attachments_count += subpass.resolveAttachments.size();
        preserve_attachments_count += subpass.preserveAttachments.size();
    }
    
    std::vector<VkAttachmentReference> color_attachments;
    std::vector<VkAttachmentReference> input_attachments;
    std::vector<VkAttachmentReference> resolve_attachments;
    std::vector<uint32_t> preserve_attachments;
    color_attachments.reserve(color_attachments_count);
    input_attachments.reserve(input_attachments_count);
    resolve_attachments.reserve(resolve_attachments_count);
    preserve_attachments.reserve(preserve_attachments_count);

    for (auto& subpass : descriptor.subpasses) {
        VkSubpassDescription sub{};

        sub.pColorAttachments =
            color_attachments.data() + color_attachments.size();
        for (auto& attachment : subpass.colorAttachments) {
            VkAttachmentReference att{};
            att.attachment = attachment.attachment;
            att.layout = ImageLayout2Vk(attachment.layout);
            color_attachments.emplace_back(att);
        }
        sub.colorAttachmentCount = subpass.colorAttachments.size();

        sub.pInputAttachments =
            input_attachments.data() + input_attachments.size();
        for (auto& attachment : subpass.inputAttachments) {
            VkAttachmentReference att{};
            att.attachment = attachment.attachment;
            att.layout = ImageLayout2Vk(attachment.layout);
            input_attachments.emplace_back(att);
        }
        sub.inputAttachmentCount = subpass.inputAttachments.size();

        sub.pResolveAttachments =
            resolve_attachments.data() + resolve_attachments.size();
        for (auto& attachment : subpass.resolveAttachments) {
            VkAttachmentReference att{};
            att.attachment = attachment.attachment;
            att.layout = ImageLayout2Vk(attachment.layout);
            resolve_attachments.emplace_back(att);
        }
        sub.preserveAttachmentCount = subpass.preserveAttachments.size();

        VkAttachmentReference depthStencilAttachment{};
        if (subpass.depthStencilAttachment) {
            depthStencilAttachment.attachment =
                subpass.depthStencilAttachment->attachment;
            depthStencilAttachment.layout =
                ImageLayout2Vk(subpass.depthStencilAttachment->layout);
            sub.pDepthStencilAttachment = &depthStencilAttachment;
        }

        sub.pPreserveAttachments =
            preserve_attachments.data() + preserve_attachments.size();
        for (auto& attachment : subpass.preserveAttachments) {
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

}  // namespace nickel::graphics