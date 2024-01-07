#include "rhi/vulkan/framebuffer.hpp"
#include "rhi/vulkan/device.hpp"

namespace nickel::rhi::vulkan {

Framebuffer::Framebuffer(Device* device, uint32_t w, uint32_t h,
                         const std::vector<ImageView>& imageViews,
                         uint32_t layers, const RenderPass& renderPass)
    : device_(device) {
    std::vector<vk::ImageView> views;
    for (auto& view : imageViews) {
        views.emplace_back(view.Raw());
    }
    vk::FramebufferCreateInfo createInfo;
    createInfo.setRenderPass(renderPass.Raw())
        .setWidth(w)
        .setHeight(h)
        .setLayers(layers)
        .setAttachments(views);
    fbo_ = device_->Raw().createFramebuffer(createInfo);
    if (fbo_) {
        LOGE(log_tag::Vulkan, "create framebuffer failed");
    }
}

Framebuffer::~Framebuffer() {
    if (device_ && fbo_) {
        device_->Raw().destroyFramebuffer(fbo_);
    }
}

}  // namespace nickel::rhi::vulkan