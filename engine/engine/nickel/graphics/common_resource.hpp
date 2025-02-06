#pragma once
#include "nickel/graphics/lowlevel/device.hpp"
#include "nickel/graphics/lowlevel/image.hpp"
#include "nickel/video/window.hpp"

namespace nickel::graphics {
class CommonResource {
public:
    CommonResource(Device device, video::Window& window);
    ImageView GetDepthImageView(uint32_t idx);
    Framebuffer GetFramebuffer(uint32_t idx);

    std::vector<Image> m_depth_images;
    std::vector<ImageView> m_depth_image_views;
    std::vector<Framebuffer> m_fbos;
    RenderPass m_render_pass;

private:
    void initDepthImages(Device& device, video::Window& window);
    void initFramebuffers(Device& devcie);
    void initRenderPass(Device& device);
};
}
