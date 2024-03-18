#include "graphics/rhi/vk/framebuffer.hpp"
#include "graphics/rhi/vk/renderpass.hpp"
#include "graphics/rhi/vk/texture_view.hpp"

namespace nickel::rhi::vulkan {

FramebufferImpl::FramebufferImpl(vk::Device dev,
                                 const std::vector<vk::ImageView>& views,
                                 const Extent3D& extent,
                                 vk::RenderPass renderPass)
    : dev_{dev}, views_{views}, extent_{extent} {
    vk::FramebufferCreateInfo info;

    info.setWidth(extent.width)
        .setHeight(extent.height)
        .setLayers(extent.depthOrArrayLayers)
        .setAttachments(views_)
        .setRenderPass(renderPass);
    VK_CALL(fbo, dev.createFramebuffer(info));
}

FramebufferImpl::~FramebufferImpl() {
    dev_.destroyFramebuffer(fbo);
}

}  // namespace nickel::rhi::vulkan