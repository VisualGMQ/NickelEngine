#pragma once
#include "common_resource.hpp"
#include "nickel/graphics/lowlevel/adapter.hpp"
#include "nickel/graphics/lowlevel/bind_group_layout.hpp"
#include "nickel/graphics/lowlevel/graphics_pipeline.hpp"
#include "nickel/graphics/lowlevel/render_pass.hpp"
#include "nickel/video/window.hpp"

namespace nickel::graphics {

class ImGuiRenderPass {
public:
    ImGuiRenderPass();
    ~ImGuiRenderPass();

    void Begin();
    void ApplyDrawCall(CommonResource&);
    
private:
    VkRenderPass m_render_pass;
    VkDescriptorPool m_descriptor_pool;
    VkPipeline m_pipeline;

    void initImGui(const video::Window& window, const Adapter& adapter);
    void initImGuiDescriptorPool(const Adapter& adapter);
    void initImGuiRenderPass(const Adapter& adapter);
    void renderImGui(Device, CommonResource&, int swapchain_image_index);
    void shutdownImGui();
};

}