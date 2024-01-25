#include "vulkan/renderpass.hpp"
#include "vulkan/device.hpp"
#include "vulkan/util.hpp"


namespace nickel::vulkan {

RenderPass::RenderPass(Device* device,
                       const std::vector<vk::AttachmentDescription>& attachmentDescs,
                       const std::vector<vk::SubpassDescription>& subpasses,
                       const std::vector<vk::SubpassDependency>& subpassDeps)
    : device_{device} {
    vk::RenderPassCreateInfo info;
    info.setSubpasses(subpasses)
        .setAttachments(attachmentDescs)
        .setDependencies(subpassDeps);
    VK_CALL(renderPass_, device->GetDevice().createRenderPass(info));
}

RenderPass::~RenderPass() {
    if (device_) {
        device_->GetDevice().destroyRenderPass(renderPass_);
    }
}

}  // namespace nickel::vulkan