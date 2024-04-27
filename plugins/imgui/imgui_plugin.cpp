#include "imgui_plugin.hpp"

#include "SDL.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "imgui_internal.h"

#ifdef NICKEL_HAS_VULKAN
#include "graphics/rhi/gl4/texture_view.hpp"
#include "graphics/rhi/vk/adapter.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/queue.hpp"
#include "graphics/rhi/vk/texture.hpp"
#include "graphics/rhi/vk/texture_view.hpp"
#include "graphics/rhi/vk/queue.hpp"
#include "graphics/rhi/vk/util.hpp"
#include "imgui_impl_vulkan.h"
#include "graphics/context.hpp"


#endif
#include "GraphEditor.h"
#include "ImCurveEdit.h"
#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "ImZoomSlider.h"

namespace plugin {

using namespace nickel;

void imguiProcessEvent(const SDL_Event& event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
}

void CheckVkResult(VkResult err) {
    if (err == 0) return;
    LOGE(stderr, "[imgui-vulkan] Error: VkResult = %d\n", err);
    if (err < 0) abort();
}

#ifdef NICKEL_HAS_VULKAN
ImGuiVkContext::ImGuiVkContext(rhi::Device device) : device_{device} {
    auto vkDevice = static_cast<rhi::vulkan::DeviceImpl*>(device.Impl());
    initDescriptorPool(vkDevice->device);
    initSetLayout(vkDevice->device);
    initDescriptorSets(vkDevice->device);
    initRenderPass(*vkDevice);
    initFramebuffers(*vkDevice);
    initCombinedSampler(vkDevice->device);
}

ImGuiVkContext::~ImGuiVkContext() {
    auto device = static_cast<rhi::vulkan::DeviceImpl*>(device_.Impl())->device;
    device.destroySampler(combinedSampler);
    for (auto fbo : framebuffers) {
        device.destroyFramebuffer(fbo);
    }
    for (auto set : descriptorSets) {
        device.freeDescriptorSets(descriptorPool, set);
    }
    device.destroyDescriptorSetLayout(descriptorSetLayout);
    device.destroyDescriptorPool(descriptorPool);
    device.destroyRenderPass(renderPass);
}

void ImGuiVkContext::initSetLayout(vk::Device device) {
    vk::DescriptorSetLayoutBinding binding;
    vk::DescriptorSetLayoutCreateInfo info;
    binding.setBinding(0)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setStageFlags(vk::ShaderStageFlagBits::eFragment);
    info.setBindingCount(0).setBindings(binding);

    VK_CALL(descriptorSetLayout, device.createDescriptorSetLayout(info));
}

void ImGuiVkContext::initCombinedSampler(vk::Device device) {
    vk::SamplerCreateInfo info;
    VK_CALL(combinedSampler, device.createSampler(info));
}

void ImGuiVkContext::initDescriptorSets(vk::Device device) {
    vk::DescriptorSetAllocateInfo allocInfo;
    std::array<vk::DescriptorSetLayout, MAX_DESCRIPTOR_SET_FOR_TEXTURE> layouts;
    layouts.fill(descriptorSetLayout);
    allocInfo.setDescriptorPool(descriptorPool)
        .setDescriptorSetCount(MAX_DESCRIPTOR_SET_FOR_TEXTURE)
        .setSetLayouts(layouts);

    auto vkDevice =
        static_cast<nickel::rhi::vulkan::DeviceImpl*>(device_.Impl());
    std::vector<vk::DescriptorSet> sets;
    VK_CALL(descriptorSets, vkDevice->device.allocateDescriptorSets(allocInfo));
}

vk::DescriptorSet ImGuiVkContext::GetTextureBindedDescriptorSet(
    const Texture& texture) {
    vk::DescriptorImageInfo descImage;
    descImage.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImageView(static_cast<const nickel::rhi::vulkan::TextureViewImpl*>(
                          texture.View().Impl())
                          ->GetView())
        .setSampler(combinedSampler);

    auto set = descriptorSets[curDescriptorSetIndex];
    curDescriptorSetIndex = (curDescriptorSetIndex + 1) % descriptorSets.size();

    vk::WriteDescriptorSet writeDesc;
    writeDesc.setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setDstSet(set)
        .setDstBinding(0)
        .setImageInfo(descImage);
    auto vkDevice =
        static_cast<nickel::rhi::vulkan::DeviceImpl*>(device_.Impl());
    vkDevice->device.updateDescriptorSets(writeDesc, {});

    return set;
}

void ImGuiVkContext::RecreateFramebuffers(rhi::vulkan::DeviceImpl& device) {
    for (auto fbo : framebuffers) {
        device.device.destroyFramebuffer(fbo);
    }

    framebuffers.clear();
    initFramebuffers(device);

    device.WaitIdle();
}

void ImGuiVkContext::initDescriptorPool(vk::Device device) {
    vk::DescriptorPoolSize poolSize(vk::DescriptorType::eCombinedImageSampler,
                                    1);
    vk::DescriptorPoolCreateInfo info;
    info.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
        .setMaxSets(1 + MAX_DESCRIPTOR_SET_FOR_TEXTURE)
        .setPoolSizes(poolSize);
    VK_CALL(descriptorPool, device.createDescriptorPool(info));
}

void ImGuiVkContext::initRenderPass(rhi::vulkan::DeviceImpl& device) {
    vk::AttachmentDescription attachment;
    attachment.setFormat(device.swapchain.imageInfo.format.format)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eLoad)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);

    vk::AttachmentReference color_attachment;
    color_attachment.setAttachment(0).setLayout(
        vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpass;
    subpass.setColorAttachments(color_attachment);

    vk::SubpassDependency dependency;
    dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
        .setDstSubpass(0)
        .setSrcStageMask(vk::PipelineStageFlagBits::eFragmentShader)
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

    vk::RenderPassCreateInfo info;
    info.setAttachmentCount(1)
        .setAttachments(attachment)
        .setSubpasses(subpass)
        .setDependencies(dependency);
    VK_CALL(renderPass, device.device.createRenderPass(info));
}

void ImGuiVkContext::initFramebuffers(rhi::vulkan::DeviceImpl& device) {
    vk::FramebufferCreateInfo info;
    info.setRenderPass(renderPass)
        .setWidth(device.swapchain.imageInfo.extent.width)
        .setHeight(device.swapchain.imageInfo.extent.height)
        .setLayers(1);
    framebuffers.resize(device.swapchain.imageInfo.imagCount);
    for (uint32_t i = 0; i < device.swapchain.imageInfo.imagCount; i++) {
        info.setAttachments(device.swapchain.ImageViews()[i]);
        VK_CALL(framebuffers[i], device.device.createFramebuffer(info));
    }
}

// get command buffer related by swapchain image
vk::CommandBuffer getCmdBuf(RenderContext& ctx) {
    return static_cast<rhi::vulkan::CommandEncoderImpl*>(ctx.encoder.Impl())
        ->buf;
}

void renderVkFrame(RenderContext& ctx, ImGuiVkContext& vkCtx,
                   rhi::vulkan::DeviceImpl& device, ImDrawData* draw_data) {
    auto idx = device.curFrame;
    auto imageAvalibleSem = device.imageAvaliableSems[idx];
    auto renderFinishSem = device.renderFinishSems[idx];
    auto cmd = getCmdBuf(ctx);

    vk::RenderPassBeginInfo renderPassInfo;
    renderPassInfo.setRenderPass(vkCtx.renderPass)
        .setFramebuffer(vkCtx.framebuffers[device.curImageIndex])
        .setRenderArea({
            {                                      0,0                                                   },
            {device.swapchain.imageInfo.extent.width,
             device.swapchain.imageInfo.extent.height}
    });
    cmd.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    ImGui_ImplVulkan_RenderDrawData(draw_data, cmd);

    vk::FramebufferCreateInfo info;

    cmd.endRenderPass();
}

void ImGuiOnWindowResize(const WindowResizeEvent& event,
                         gecs::resource<rhi::Adapter> adapter,
                         gecs::resource<rhi::Device> device,
                         gecs::resource<gecs::mut<ImGuiVkContext>> ctx,
                         gecs::resource<Window> window) {
    if (adapter->RequestAdapterInfo().api != rhi::APIPreference::Vulkan) {
        return;
    }

    auto size = window->Size();
    if (size.w == 0 || size.h == 0) {
        return;
    }

    auto vkDevice = static_cast<rhi::vulkan::DeviceImpl*>(device->Impl());
    auto vkAdapter = static_cast<rhi::vulkan::AdapterImpl*>(adapter->Impl());

    ctx->RecreateFramebuffers(*vkDevice);

    ImGui_ImplVulkan_SetMinImageCount(vkDevice->swapchain.imageInfo.imagCount);
}
#endif

void ImGuiInit(gecs::commands cmds, gecs::resource<gecs::mut<Window>> window,
               gecs::resource<gecs::mut<EventPoller>> poller,
               gecs::resource<rhi::Adapter> adapter,
               gecs::resource<rhi::Device> device,
               gecs::event_dispatcher<WindowResizeEvent> eventDispatcher) {
    PROFILE_BEGIN();

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

        auto& ctx = cmds.emplace_resource<ImGuiVkContext>(device.get());

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
        init_info.DescriptorPool = ctx.descriptorPool;
        init_info.RenderPass = ctx.renderPass;
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
    eventDispatcher.immediatly_sink().add<ImGuiOnWindowResize>();

    PROFILE_END();
}

void ImGuiGameWindowLayoutTransition(
    gecs::resource<gecs::mut<rhi::Device>> device,
    gecs::resource<gecs::mut<Camera>> camera,
    gecs::resource<gecs::mut<RenderContext>> renderCtx) {
    PROFILE_BEGIN();

    auto cmd = getCmdBuf(renderCtx.get());

    auto target = camera->GetTarget();

    if (!target) {
        return ;
    }

    auto texture = static_cast<rhi::vulkan::TextureImpl*>(target.Impl()->Texture().Impl());

    auto dev = static_cast<rhi::vulkan::DeviceImpl*>(device->Impl());

    dev->WaitIdle();

    auto aspect = nickel::rhi::vulkan::DetermineTextureAspect(
        nickel::rhi::TextureAspect::All, texture->Format());

    vk::ImageMemoryBarrier barrier;
    vk::ImageSubresourceRange range;
    range.setAspectMask(aspect)
        .setBaseArrayLayer(0)
        .setLayerCount(1)
        .setBaseMipLevel(0)
        .setLevelCount(1);

    barrier.setOldLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setSrcAccessMask(vk::AccessFlagBits::eNone)
        .setDstAccessMask(vk::AccessFlagBits::eNone)
        .setImage(texture->GetImage())
        .setSubresourceRange(range);

    cmd.pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe,
                        vk::PipelineStageFlagBits::eBottomOfPipe,
                        vk::DependencyFlagBits::eByRegion, {}, {}, barrier);

    texture->layouts[0] = vk::ImageLayout::eShaderReadOnlyOptimal;
}

void ImGuiStart(gecs::resource<rhi::Adapter> adapter) {
    PROFILE_BEGIN();

    if (adapter->RequestAdapterInfo().api == rhi::APIPreference::GL) {
        ImGui_ImplOpenGL3_NewFrame();
    } else {
#ifdef NICKEL_HAS_VULKAN
        ImGui_ImplVulkan_NewFrame();
#endif
    }
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    PROFILE_END();
}

void ImGuiEnd(gecs::resource<gecs::mut<Window>> window,
              gecs::resource<rhi::Adapter> adapter,
              gecs::resource<rhi::Device> device,
              gecs::resource<gecs::mut<ImGuiVkContext>> vkCtx,
              gecs::resource<gecs::mut<RenderContext>> ctx) {
    PROFILE_BEGIN();

    ImGui::Render();

    if (adapter->RequestAdapterInfo().api == rhi::APIPreference::GL) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    } else {
#ifdef NICKEL_HAS_VULKAN
        ImDrawData* main_draw_data = ImGui::GetDrawData();
        renderVkFrame(ctx.get(), vkCtx.get(),
                      *static_cast<rhi::vulkan::DeviceImpl*>(device->Impl()),
                      main_draw_data);
#endif
    }

    PROFILE_END();
}

void ImGuiShutdown(gecs::commands cmds, gecs::resource<rhi::Adapter> adapter,
                   gecs::resource<rhi::Device> device) {
    PROFILE_BEGIN();

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
        cmds.remove_resource<ImGuiVkContext>();
    }
#endif

    PROFILE_END();
}

}  // namespace plugin

namespace ImGui {

void Image(const ::nickel::Texture& texture, const ImVec2& image_size,
           const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col,
           const ImVec4& border_col) {
    auto api = nickel::ECS::Instance()
                   .World()
                   .res_mut<nickel::rhi::Adapter>()
                   ->RequestAdapterInfo()
                   .api;
#ifdef NICKEL_HAS_VULKAN
    if (api == nickel::rhi::APIPreference::Vulkan) {
        auto ctx =
            nickel::ECS::Instance().World().res_mut<plugin::ImGuiVkContext>();
        ::ImGui::Image(ctx->GetTextureBindedDescriptorSet(texture), image_size,
                       uv0, uv1, tint_col, border_col);
    }
#endif
    if (api == nickel::rhi::APIPreference::GL) {
        auto glTexture =
            static_cast<const ::nickel::rhi::gl4::TextureViewImpl*>(
                texture.View().Impl());
        ::ImGui::Image((ImTextureID)glTexture->id, image_size, uv0, uv1,
                       tint_col, border_col);
    }
}

bool ImageButton(const char* str_id, const ::nickel::Texture& texture,
                 const ImVec2& image_size, const ImVec2& uv0, const ImVec2& uv1,
                 const ImVec4& bg_col, const ImVec4& tint_col) {
    auto api = nickel::ECS::Instance()
                   .World()
                   .res_mut<nickel::rhi::Adapter>()
                   ->RequestAdapterInfo()
                   .api;
#ifdef NICKEL_HAS_VULKAN
    if (api == nickel::rhi::APIPreference::Vulkan) {
        auto ctx =
            nickel::ECS::Instance().World().res_mut<plugin::ImGuiVkContext>();
        return ::ImGui::ImageButton(str_id,
                                    ctx->GetTextureBindedDescriptorSet(texture),
                                    image_size, uv0, uv1, bg_col, tint_col);
    }
#endif
    if (api == nickel::rhi::APIPreference::GL) {
        auto glTexture =
            static_cast<const ::nickel::rhi::gl4::TextureViewImpl*>(
                texture.View().Impl());
        return ::ImGui::ImageButton(str_id, (ImTextureID)glTexture->id,
                                    image_size, uv0, uv1, bg_col, tint_col);
    }
    return false;
}

bool ImageButton(const ::nickel::Texture& texture, const ImVec2& size,
                 const ImVec2& uv0, const ImVec2& uv1, int frame_padding,
                 const ImVec4& bg_col, const ImVec4& tint_col) {
    auto api = nickel::ECS::Instance()
                   .World()
                   .res_mut<nickel::rhi::Adapter>()
                   ->RequestAdapterInfo()
                   .api;
#ifdef NICKEL_HAS_VULKAN
    if (api == nickel::rhi::APIPreference::Vulkan) {
        auto ctx =
            nickel::ECS::Instance().World().res_mut<plugin::ImGuiVkContext>();
        return ::ImGui::ImageButton(ctx->GetTextureBindedDescriptorSet(texture),
                                    size, uv0, uv1, frame_padding, bg_col,
                                    tint_col);
    }
#endif
    if (api == nickel::rhi::APIPreference::GL) {
        auto glTexture =
            static_cast<const ::nickel::rhi::gl4::TextureViewImpl*>(
                texture.View().Impl());
        return ::ImGui::ImageButton((ImTextureID)glTexture->id, size, uv0, uv1,
                                    frame_padding, bg_col, tint_col);
    }
    return false;
}

}  // namespace ImGui