#include "imgui_plugin.hpp"

#include "SDL.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#ifdef NICKEL_HAS_VULKAN
#include "imgui_impl_vulkan.h"
#include "graphics/rhi/vk/adapter.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/queue.hpp"
#include "graphics/rhi/vk/util.hpp"
#endif
#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "ImZoomSlider.h"
#include "ImCurveEdit.h"
#include "GraphEditor.h"

namespace plugin {

using namespace nickel;

void imguiProcessEvent(const SDL_Event& event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
}

void CheckVkResult(VkResult err) {
    if (err == 0)
        return;
    LOGE(stderr, "[imgui-vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}


#ifdef NICKEL_HAS_VULKAN
struct ImGuiVkContext {
    vk::DescriptorPool descriptorPool;
    vk::RenderPass renderPass;
    std::vector<vk::Framebuffer> framebuffers;

    void Init(rhi::Device device) {
        auto vkDevice = static_cast<rhi::vulkan::DeviceImpl*>(device.Impl());
        descriptorPool = createDescriptorPool(vkDevice->device);
        renderPass = createRenderPass(*vkDevice);
        framebuffers = createFramebuffers(*vkDevice);
    }

    void Destory(rhi::Device dev) {
        auto device = static_cast<rhi::vulkan::DeviceImpl*>(dev.Impl())->device;
        device.destroyDescriptorPool(descriptorPool);
        for (auto fbo : framebuffers) {
            device.destroyFramebuffer(fbo);
        }
        device.destroyRenderPass(renderPass);
    }

private:
    vk::DescriptorPool createDescriptorPool(vk::Device device) {
        vk::DescriptorPoolSize poolSize(
            vk::DescriptorType::eCombinedImageSampler, 1);
        vk::DescriptorPoolCreateInfo info;
        info.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
            .setMaxSets(1)
            .setPoolSizes(poolSize);
        vk::DescriptorPool pool;
        VK_CALL(pool, device.createDescriptorPool(info));
        return pool;
    }

    vk::RenderPass createRenderPass(rhi::vulkan::DeviceImpl& device) {
        vk::AttachmentDescription attachment;
        attachment.setFormat(device.swapchain.imageInfo.format.format)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eLoad)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);

        vk::AttachmentReference color_attachment;
        color_attachment.setAttachment(0).setLayout(
            vk::ImageLayout::eColorAttachmentOptimal);

        vk::SubpassDescription subpass;
        subpass.setColorAttachments(color_attachment);

        vk::SubpassDependency dependency;
        dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
            .setDstSubpass(0)
            .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

        vk::RenderPassCreateInfo info;
        info.setAttachmentCount(1)
            .setAttachments(attachment)
            .setSubpasses(subpass)
            .setDependencies(dependency);
        vk::RenderPass renderPass;
        VK_CALL(renderPass, device.device.createRenderPass(info));

        return renderPass;
    }

    std::vector<vk::Framebuffer> createFramebuffers(rhi::vulkan::DeviceImpl& device) {
        vk::FramebufferCreateInfo info;
        info.setRenderPass(renderPass)
            .setWidth(device.swapchain.imageInfo.extent.width)
            .setHeight(device.swapchain.imageInfo.extent.height)
            .setLayers(1);
        std::vector<vk::Framebuffer> fbos(device.swapchain.imageInfo.imagCount);
        for (uint32_t i = 0; i < device.swapchain.imageInfo.imagCount; i++) {
            info.setAttachments(device.swapchain.ImageViews()[i]);
            VK_CALL(fbos[i], device.device.createFramebuffer(info));
        }
        return fbos;
    }
} gImGuiVkContext;

// get command buffer related by swapchain image
vk::CommandBuffer getCmdBuf(RenderContext& ctx) {
    return static_cast<rhi::vulkan::CommandEncoderImpl*>(ctx.encoder.Impl())->buf;
}

void renderVkFrame(RenderContext& ctx, rhi::vulkan::DeviceImpl& device, ImDrawData* draw_data) {
    auto idx = device.curFrame;
    auto imageAvalibleSem = device.imageAvaliableSems[idx];
    auto renderFinishSem = device.renderFinishSems[idx];
    auto cmd = getCmdBuf(ctx);

    vk::RenderPassBeginInfo renderPassInfo;
    renderPassInfo.setRenderPass(gImGuiVkContext.renderPass)
        .setFramebuffer(gImGuiVkContext.framebuffers[device.curImageIndex])
        .setRenderArea({
            {                                      0,0                                                   },
            {device.swapchain.imageInfo.extent.width,
             device.swapchain.imageInfo.extent.height}
    });
    cmd.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    // Record dear imgui primitives into command buffer
    ImGui_ImplVulkan_RenderDrawData(draw_data, cmd);

    cmd.endRenderPass();

    // VK_CALL_NO_VALUE(cmd.end());

    // VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // vk::SubmitInfo submit;
    // vk::PipelineStageFlags waitDstStage =
    //     vk::PipelineStageFlagBits::eColorAttachmentOutput;
    // submit.setWaitSemaphores(imageAvalibleSem)
    //     .setSignalSemaphores(renderFinishSem)
    //     .setCommandBuffers(cmd)
    //     .setWaitDstStageMask(waitDstStage);

    // VK_CALL_NO_VALUE(
    //     static_cast<rhi::vulkan::QueueImpl*>(device.graphicsQueue->Impl())
    //         ->queue.submit(submit));
}
#endif

void ImGuiInit(gecs::resource<gecs::mut<Window>> window,
               gecs::resource<gecs::mut<EventPoller>> poller,
               gecs::resource<rhi::Adapter> adapter,
               gecs::resource<rhi::Device> device) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // enable Docking

    ImGui::StyleColorsDark();

    if (adapter->RequestAdapterInfo().api == rhi::APIPreference::GL) {
        ImGui_ImplSDL2_InitForOpenGL((SDL_Window*)window->Raw(), nullptr);
        ImGui_ImplOpenGL3_Init("#version 430");
    } else {
#ifdef NICKEL_HAS_VULKAN
        ImGui_ImplSDL2_InitForVulkan((SDL_Window*)window->Raw());
        ImGui_ImplVulkan_InitInfo init_info = {};

        gImGuiVkContext.Init(device.get());

        auto vkAdapter =
            static_cast<rhi::vulkan::AdapterImpl*>(adapter->Impl());
        auto vkDevice = static_cast<rhi::vulkan::DeviceImpl*>(device->Impl());

        init_info.Instance = vkAdapter->instance;
        init_info.PhysicalDevice = vkAdapter->phyDevice;
        init_info.Device = vkDevice->device;
        init_info.QueueFamily = vkDevice->queueIndices.graphicsIndex.value();
        init_info.Queue =
            static_cast<rhi::vulkan::QueueImpl*>(vkDevice->GetQueue().Impl())
                ->queue;
        init_info.PipelineCache = nullptr;
        init_info.DescriptorPool = gImGuiVkContext.descriptorPool;
        init_info.RenderPass = gImGuiVkContext.renderPass;
        init_info.Subpass = 0;
        init_info.MinImageCount = vkDevice->swapchain.imageInfo.imagCount;
        init_info.ImageCount = vkDevice->swapchain.imageInfo.imagCount;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = CheckVkResult;
        ImGui_ImplVulkan_Init(&init_info);
#endif
    }

    poller->InjectHandler<imguiProcessEvent>();
}

void ImGuiStart(gecs::resource<rhi::Adapter> adapter) {
    if (adapter->RequestAdapterInfo().api == rhi::APIPreference::GL) {
        ImGui_ImplOpenGL3_NewFrame();
    } else {
#ifdef NICKEL_HAS_VULKAN
        ImGui_ImplVulkan_NewFrame();
#endif
    }
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void ImGuiEnd(gecs::resource<gecs::mut<Window>> window,
              gecs::resource<rhi::Adapter> adapter,
              gecs::resource<rhi::Device> device,
              gecs::resource<gecs::mut<RenderContext>> ctx) {
    ImGui::Render();

    if (adapter->RequestAdapterInfo().api == rhi::APIPreference::GL) {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    } else {
#ifdef NICKEL_HAS_VULKAN
        ImDrawData* main_draw_data = ImGui::GetDrawData();
        renderVkFrame(ctx.get(), *static_cast<rhi::vulkan::DeviceImpl*>(device->Impl()),
                      main_draw_data);
#endif
    }
}

void ImGuiShutdown(gecs::resource<rhi::Adapter> adapter,
                   gecs::resource<rhi::Device> device) {
    if (adapter->RequestAdapterInfo().api == rhi::APIPreference::GL) {
        ImGui_ImplOpenGL3_Shutdown();
    } else {
#ifdef NICKEL_HAS_VULKAN
        ImGui_ImplVulkan_Shutdown();
#endif
    }
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

#ifdef NICKEL_HAS_VULKAN
    if (adapter->RequestAdapterInfo().api == rhi::APIPreference::Vulkan) {
        gImGuiVkContext.Destory(device.get());
    }
#endif
}

}  // namespace plugin