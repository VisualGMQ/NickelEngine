#include "graphics/rhi/vk/renderpass.hpp"
#include "graphics/rhi/vk/convert.hpp"
#include "graphics/rhi/vk/texture_view.hpp"
#include "graphics/rhi/vk/device.hpp"

namespace nickel::rhi::vulkan {

RenderPassImpl::RenderPassImpl(DeviceImpl& dev,
                               const RenderPass::Descriptor& desc)
    : dev_{dev.device}, desc_{desc} {
    createRenderPass(dev, desc);
}

void RenderPassImpl::createRenderPass(DeviceImpl& dev, const RenderPass::Descriptor& desc) {
    vk::RenderPassCreateInfo info;
    std::vector<vk::AttachmentDescription> attachments;
    size_t attCount = desc.colorAttachments.size() +
                      (desc.depthStencilAttachment.has_value() ? 1 : 0);
    attachments.reserve(attCount);
    std::vector<vk::AttachmentReference> attRefs;
    attRefs.reserve(attCount);

    for (int i = 0; i < desc.colorAttachments.size(); i++) {
        auto& colorAtt = desc.colorAttachments[i];
        vk::AttachmentDescription attDesc;
        attDesc.setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(GetLayoutByFormat(colorAtt.view.Format()))
            .setFormat(colorAtt.view.Format() == Format::Presentation
                           ? dev.swapchain.imageInfo.format.format
                           : Format2Vk(colorAtt.view.Format()))
            .setLoadOp(AttachmentLoadOp2Vk(colorAtt.loadOp))
            .setStoreOp(AttachmentStoreOp2Vk(colorAtt.storeOp))
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eNone)
            .setSamples(SampleCount2Vk(colorAtt.view.Texture().SampleCount()));

        attachments.push_back(attDesc);

        vk::AttachmentReference ref;
        ref.setAttachment(i).setLayout(
            vk::ImageLayout::eColorAttachmentOptimal);
        attRefs.emplace_back(ref);
    }

    if (desc.depthStencilAttachment) {
        vk::AttachmentDescription depthAttach;

        depthAttach.setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(
                GetLayoutByFormat(desc.depthStencilAttachment->view.Format(),
                                  desc.depthStencilAttachment->stencilReadOnly,
                                  desc.depthStencilAttachment->depthReadOnly))
            .setFormat(Format2Vk(desc.depthStencilAttachment->view.Format()))
            .setLoadOp(
                AttachmentLoadOp2Vk(desc.depthStencilAttachment->depthLoadOp))
            .setStoreOp(
                AttachmentStoreOp2Vk(desc.depthStencilAttachment->depthStoreOp))
            .setStencilLoadOp(
                AttachmentLoadOp2Vk(desc.depthStencilAttachment->stencilLoadOp))
            .setStencilStoreOp(AttachmentStoreOp2Vk(
                desc.depthStencilAttachment->stencilStoreOp))
            .setSamples(SampleCount2Vk(
                desc.depthStencilAttachment->view.Texture().SampleCount()));

        attachments.push_back(depthAttach);

        vk::AttachmentReference ref;

        ref.setAttachment(desc.colorAttachments.size())
            .setLayout(depthAttach.finalLayout);
        attRefs.emplace_back(ref);
    }

    vk::SubpassDescription subpass;
    subpass.setColorAttachments(attRefs).setPipelineBindPoint(
        vk::PipelineBindPoint::eGraphics);
    if (desc.depthStencilAttachment && !attRefs.empty()) {
        subpass.setPDepthStencilAttachment(&attRefs.back());
    }

    vk::SubpassDependency dep;
    dep.setSrcSubpass(VK_SUBPASS_EXTERNAL).setDstSubpass(0);
    if (desc.depthStencilAttachment) {
        dep.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput |
                            vk::PipelineStageFlagBits::eEarlyFragmentTests)
            .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput |
                             vk::PipelineStageFlagBits::eEarlyFragmentTests)
            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite |
                              vk::AccessFlagBits::eDepthStencilAttachmentWrite);
    } else {
        dep.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
    }

    info.setAttachments(attachments).setSubpasses(subpass).setDependencies(dep);

    VK_CALL(renderPass, dev_.createRenderPass(info));
}

RenderPassImpl::~RenderPassImpl() {
    if (renderPass) {
        dev_.destroyRenderPass(renderPass);
    }
}

}  // namespace nickel::rhi::vulkan