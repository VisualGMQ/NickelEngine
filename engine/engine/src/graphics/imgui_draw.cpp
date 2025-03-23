#include "nickel/graphics/imgui_draw.hpp"

#include "nickel/common/macro.hpp"
#include "nickel/graphics/lowlevel/internal/adapter_impl.hpp"
#include "nickel/graphics/lowlevel/internal/cmd_impl.hpp"
#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/enum_convert.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"
#include "nickel/nickel.hpp"
#include "nickel/video/internal/window_impl.hpp"

namespace nickel::graphics {

ImGuiRenderPass::ImGuiRenderPass(const video::Window& window,
                                 const Adapter& adapter)
    : m_device{adapter.GetDevice().Impl().m_device} {
    initDescriptorPool(adapter);
    initRenderPass(adapter);
    initCmdPool(adapter.GetDevice());
    InitFramebuffers(adapter.GetDevice());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
    io.ConfigFlags |=
        ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport / Platform
                                           // Windows
    ImFontConfig font_config;
    font_config.SizePixels = 30;
    io.Fonts->AddFontDefault(&font_config);

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;

    DeviceImpl& impl = adapter.GetDevice().Impl();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForVulkan(window.GetImpl().m_window);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = adapter.GetImpl().m_instance;
    init_info.PhysicalDevice = adapter.GetImpl().m_phy_device;
    init_info.Device = impl.m_device;
    init_info.QueueFamily = impl.m_queue_indices.m_graphics_index.value();
    init_info.Queue = impl.m_graphics_queue;
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = m_descriptor_pool;
    init_info.RenderPass = m_render_pass;
    init_info.Subpass = 0;
    init_info.MinImageCount = impl.GetSwapchainImageInfo().m_image_count;
    init_info.ImageCount = impl.GetSwapchainImageInfo().m_image_count;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = nullptr;
    init_info.CheckVkResultFn = [](VkResult result) {
        if (result != VK_SUCCESS) {
            LOGE("ImGui Vulkan Error: {}", VkError2String(result));
        }
    };
    ImGui_ImplVulkan_Init(&init_info);
}

ImGuiRenderPass::~ImGuiRenderPass() {
    vkDeviceWaitIdle(m_device);

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    DestroyFramebuffers();
    vkDestroyDescriptorPool(m_device, m_descriptor_pool, nullptr);
    vkDestroyRenderPass(m_device, m_render_pass, nullptr);
    VK_CALL(vkResetCommandPool(m_device, m_cmd_pool, 0));
    vkDestroyCommandPool(m_device, m_cmd_pool, nullptr);
}

void ImGuiRenderPass::Begin() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void ImGuiRenderPass::End(Device device, CommonResource& res,
                          uint32_t cur_swapchain_image_idx,
                          uint32_t cur_frame) {
    renderImGui(device, res, cur_swapchain_image_idx, cur_frame);
}

void ImGuiRenderPass::PrepareForRender() {
    ImGui::Render();
    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void ImGuiRenderPass::initDescriptorPool(const Adapter& adapter) {
    VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
         IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE},
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 0;
    for (VkDescriptorPoolSize& pool_size : pool_sizes)
        pool_info.maxSets += pool_size.descriptorCount;
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    VK_CALL(vkCreateDescriptorPool(adapter.GetDevice().Impl().m_device,
                                   &pool_info, nullptr, &m_descriptor_pool));
}

void ImGuiRenderPass::initRenderPass(const Adapter& adapter) {
    VkAttachmentDescription attachment = {};
    attachment.format = Format2Vk(
        adapter.GetDevice().GetSwapchainImageInfo().m_surface_format.format);
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    VkAttachmentReference color_attachment = {};
    color_attachment.attachment = 0;
    color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment;
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.pAttachments = &attachment;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = 1;
    info.pDependencies = &dependency;
    VK_CALL(vkCreateRenderPass(adapter.GetDevice().Impl().m_device, &info,
                               nullptr, &m_render_pass));
}

void ImGuiRenderPass::initCmdPool(const Device& device) {
    VkCommandPoolCreateInfo ci = {};
    ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    VK_CALL(
        vkCreateCommandPool(device.Impl().m_device, &ci, nullptr, &m_cmd_pool));
}

void ImGuiRenderPass::InitFramebuffers(const Device& device) {
    for (uint32_t i = 0; i < device.GetSwapchainImageInfo().m_image_count;
         i++) {
        ImageView view = device.GetSwapchainImageViews()[i];

        VkFramebufferCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.renderPass = m_render_pass;
        info.attachmentCount = 1;
        info.pAttachments = &view.GetImpl()->m_view;
        info.width = device.GetSwapchainImageInfo().m_extent.w;
        info.height = device.GetSwapchainImageInfo().m_extent.h;
        info.layers = 1;

        VkFramebuffer fbo;
        VK_CALL(
            vkCreateFramebuffer(device.Impl().m_device, &info, nullptr, &fbo));
        m_fbos.push_back(fbo);
    }
}

void ImGuiRenderPass::DestroyFramebuffers() {
    for (auto fbo : m_fbos) {
        vkDestroyFramebuffer(m_device, fbo, nullptr);
    }
    m_fbos.clear();
}

void ImGuiRenderPass::renderImGui(Device device, CommonResource& res,
                                  uint32_t cur_swapchain_image_idx,
                                  uint32_t cur_frame_idx) {
    auto draw_data = ImGui::GetDrawData();

    NICKEL_RETURN_IF_FALSE(draw_data->DisplaySize.x > 0 &&
                           draw_data->DisplaySize.y > 0);

    auto encoder = device.CreateCommandEncoder();
    VkCommandBuffer cmd = encoder.GetImpl().m_cmd;

    {
        VkRenderPassBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = m_render_pass;
        info.framebuffer = m_fbos[cur_swapchain_image_idx];
        info.renderArea.extent.width =
            device.GetSwapchainImageInfo().m_extent.w;
        info.renderArea.extent.height =
            device.GetSwapchainImageInfo().m_extent.h;
        info.clearValueCount = 0;
        vkCmdBeginRenderPass(cmd, &info, VK_SUBPASS_CONTENTS_INLINE);
    }

    // Record dear imgui primitives into command m_buffer
    ImGui_ImplVulkan_RenderDrawData(draw_data, cmd);


    // Submit command buffer
    vkCmdEndRenderPass(cmd);
    {
        VkPipelineStageFlags wait_stage =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores =
            &res.GetRenderFinishSemaphore(cur_frame_idx).GetImpl()->m_semaphore;
        info.pWaitDstStageMask = &wait_stage;
        info.commandBufferCount = 1;
        info.pCommandBuffers = &cmd;
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores =
            &res.GetImGuiRenderFinishSemaphore(cur_frame_idx)
                 .GetImpl()
                 ->m_semaphore;

        VK_CALL(vkEndCommandBuffer(cmd));
        VK_CALL(vkQueueSubmit(device.Impl().m_graphics_queue, 1, &info,
                              res.GetFence(cur_frame_idx).GetImpl()->m_fence));
    }

    encoder.GetImpl().PendingDelete();
}

}  // namespace nickel::graphics