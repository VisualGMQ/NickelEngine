#include "graphics/rhi/vk/framebuffer.hpp"
#include "graphics/rhi/vk/renderpass.hpp"
#include "graphics/rhi/vk/texture_view.hpp"

namespace nickel::rhi::vulkan {

FramebufferImpl::FramebufferImpl(vk::Device dev,
                                 const std::vector<TextureView>& views,
                                 const Extent3D& extent,
                                 vk::RenderPass renderPass)
    : dev_{dev}, views_{views}, extent_{extent} {
    vk::FramebufferCreateInfo info;

    std::vector<vk::ImageView> vkViews;
    std::transform(
        views.begin(), views.end(), std::back_inserter(vkViews),
        [](TextureView view) {
            return static_cast<TextureViewImpl*>(view.Impl())->GetView();
        });

    info.setWidth(extent.width)
        .setHeight(extent.height)
        .setLayers(extent.depthOrArrayLayers)
        .setAttachments(vkViews)
        .setRenderPass(renderPass);
    VK_CALL(fbo, dev.createFramebuffer(info));
}

FramebufferImpl::~FramebufferImpl() {
    dev_.destroyFramebuffer(fbo);
}

}  // namespace nickel::rhi::vulkan