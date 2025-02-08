#pragma once
#include "nickel/graphics/lowlevel/device.hpp"
#include "nickel/graphics/lowlevel/image.hpp"
#include "nickel/video/window.hpp"

namespace nickel::graphics {
class CommonResource {
public:
    CommonResource(Device device, const video::Window& window);
    ImageView GetDepthImageView(uint32_t idx);
    Framebuffer GetFramebuffer(uint32_t idx);
    Semaphore& GetImageAvaliableSemaphore(uint32_t idx);
    Semaphore& GetRenderFinishSemaphore(uint32_t idx);
    Semaphore& GetImGuiRenderFinishSemaphore(uint32_t idx);
    Fence& GetFence(uint32_t idx);

    std::vector<Image> m_depth_images;
    std::vector<ImageView> m_depth_image_views;
    std::vector<Framebuffer> m_fbos;
    RenderPass m_render_pass;
    std::vector<Fence> m_present_fences;
    std::vector<Semaphore> m_image_avaliable_sems;
    std::vector<Semaphore> m_render_finish_sems;
    std::vector<Semaphore> m_imgui_render_finish_sems;

private:
    void initDepthImages(Device& device, const video::Window& window);
    void initFramebuffers(Device& devcie);
    void initRenderPass(Device& device);
    void initSyncObjects(Device& device);
};
}  // namespace nickel::graphics
