#include "graphics/rhi/vk/renderpass.hpp"
#include "graphics/rhi/vk/convert.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/texture.hpp"
#include "graphics/rhi/vk/texture_view.hpp"

namespace nickel::rhi::vulkan {

RenderPassImpl::RenderPassImpl(vk::Device device,
                               const rhi::RenderPass::Descriptor& descriptor,
                               const RenderPassInfo& info)
    : renderPassInfo{std::move(info)}, dev_{device}, descriptor_{descriptor} {
    vk::RenderPassCreateInfo createInfo;

    std::vector<vk::SubpassDescription> subpasses(
        renderPassInfo.subpasses.size());
    std::transform(
        renderPassInfo.subpasses.begin(), renderPassInfo.subpasses.end(),
        subpasses.begin(), [](const SubpassInfo& info) {
            vk::SubpassDescription subpass;
            subpass.setColorAttachments(info.colorRefs);
            if (info.depthRef) {
                subpass.setPDepthStencilAttachment(&info.depthRef.value());
            }
            return subpass;
        });

    createInfo.setAttachments(renderPassInfo.descriptions)
        .setDependencies(renderPassInfo.dependencies)
        .setSubpasses(subpasses);

    VK_CALL(renderPass, dev_.createRenderPass(createInfo));
}

RenderPassImpl::~RenderPassImpl() {
    if (renderPass) {
        dev_.destroyRenderPass(renderPass);
    }
}

}  // namespace nickel::rhi::vulkan