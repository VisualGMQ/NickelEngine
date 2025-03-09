#pragma once
#include "nickel/graphics/common_resource.hpp"
#include "nickel/graphics/lowlevel/adapter.hpp"
#include "nickel/video/window.hpp"

namespace nickel::graphics {

class ImGuiRenderPass {
public:
    ImGuiRenderPass(const video::Window& window, const Adapter&);
    ~ImGuiRenderPass();

    void Begin();
    void End(Device device, CommonResource&,
             uint32_t cur_swapchain_image_idx, uint32_t cur_frame);
    void PrepareForRender();
    void InitFramebuffers(const Device&);
    void DestroyFramebuffers();

    VkDevice m_device;
    VkRenderPass m_render_pass;
    VkDescriptorPool m_descriptor_pool;
    VkPipeline m_pipeline;
    VkCommandPool m_cmd_pool;
    std::vector<VkFramebuffer> m_fbos;

private:
    void initDescriptorPool(const Adapter& adapter);
    void initRenderPass(const Adapter& adapter);
    void initCmdPool(const Device& device);
    void renderImGui(Device, CommonResource&, uint32_t cur_swapchain_image_idx,
                     uint32_t cur_frame_idx);
};

}  // namespace nickel::graphics