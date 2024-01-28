#include "nickel.hpp"
#include "vulkan/device.hpp"
#include "vulkan/util.hpp"

struct Context {
    // nickel::vulkan::Pipeline* pipeline{};
    nickel::vulkan::RenderPass* renderPass{};
    nickel::vulkan::CommandPool* cmdPool{};
    std::vector<nickel::vulkan::Framebuffer*> fbos;
    nickel::vulkan::Fence* fence{};
};

auto createPipelineLayout(nickel::vulkan::Device& device) {
    return device.CreatePipelineLayout({}, {});
}

auto createRenderPass(nickel::vulkan::Device& device) {
    auto& swapchain = device.GetSwapchain();

    vk::AttachmentDescription attach;
    attach.setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFormat(swapchain.ImageInfo().format.format)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eNone)
        .setSamples(vk::SampleCountFlagBits::e1);

    vk::SubpassDescription subpass;
    vk::AttachmentReference ref;
    ref.setLayout(vk::ImageLayout::eColorAttachmentOptimal).setAttachment(0);
    subpass.setColorAttachments(ref).setPipelineBindPoint(
        vk::PipelineBindPoint::eGraphics);

    vk::SubpassDependency dep;
    dep.setSrcSubpass(VK_SUBPASS_EXTERNAL)
        .setDstSubpass(0)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

    return device.CreateRenderPass({attach}, {subpass}, {dep});
}

auto createPipeline(nickel::vulkan::Device& device,
                    const nickel::vulkan::PipelineLayout& layout,
                    const nickel::vulkan::RenderPass& renderPass) {
    vk::PipelineInputAssemblyStateCreateInfo inputAsm;
    inputAsm.setTopology(vk::PrimitiveTopology::eTriangleList);

    auto& swapchain = device.GetSwapchain();
    auto extent = swapchain.ImageInfo().extent;
    vk::PipelineViewportStateCreateInfo viewportState;
    vk::Viewport viewport;
    viewport.setX(0)
        .setY(0)
        .setWidth(extent.width)
        .setHeight(extent.height)
        .setMinDepth(0)
        .setMaxDepth(1);
    vk::Rect2D scissor;
    scissor.setOffset({0, 0}).setExtent(extent);
    viewportState.setViewports(viewport).setScissors(scissor);

    std::vector<nickel::vulkan::ShaderModule*> modules;
    auto vertShader =
        device
            .CreateShaderModule(vk::ShaderStageFlagBits::eVertex,
                                "test/testbed/vulkan/01clearscreen/vert.spv")
            .value;
    auto fragShader =
        device
            .CreateShaderModule(vk::ShaderStageFlagBits::eFragment,
                                "test/testbed/vulkan/01clearscreen/frag.spv")
            .value;

    vk::PipelineRasterizationStateCreateInfo raster;
    raster.setLineWidth(1).setRasterizerDiscardEnable(false);

    vk::PipelineColorBlendStateCreateInfo colorBlend;
    vk::PipelineColorBlendAttachmentState blendAttach;
    blendAttach.setBlendEnable(false);
    colorBlend.setLogicOpEnable(false).setAttachments(blendAttach);

    return device.CreateGraphicsPipeline({}, inputAsm, {vertShader, fragShader},
                                         viewportState, raster, {}, {},
                                         colorBlend, layout, renderPass);
}

void StartupSystem(gecs::commands cmds,
                   gecs::resource<gecs::mut<nickel::Window>> window) {
    auto& ctx = cmds.emplace_resource<Context>();
    auto& device = cmds.emplace_resource<nickel::vulkan::Device>(window.get());
    auto layout = createPipelineLayout(device);
    ctx.renderPass = createRenderPass(device).value;
    // ctx.pipeline = createPipeline(device, *layout.value, *ctx.renderPass).value;
    ctx.cmdPool = device
                      .CreateCommandPool(
                          vk::CommandPoolCreateFlagBits::eTransient,
                          device.GetQueueFamilyIndices().graphicsIndex.value())
                      .value;

    auto& swapchain = device.GetSwapchain();
    for (auto view : swapchain.ImageViews()) {
        auto fbo =
            device
                .CreateFramebuffer({view}, swapchain.ImageInfo().extent.width,
                                   swapchain.ImageInfo().extent.height, 1,
                                   *ctx.renderPass)
                .value;
        ctx.fbos.emplace_back(fbo);
    }

    ctx.fence = device.CreateFence().value;
}

void recordCommand(vk::CommandBuffer cmd, vk::Image image) {
    vk::ClearColorValue clearValue;
    clearValue.setFloat32({1, 1, 0, 1});
    vk::ImageSubresourceRange range;
    range.setBaseArrayLayer(0)
        .setBaseMipLevel(0)
        .setLayerCount(1)
        .setLevelCount(1)
        .setAspectMask(vk::ImageAspectFlagBits::eColor);

    vk::ImageMemoryBarrier barrier;
    barrier.setImage(image)
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
        .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setSubresourceRange(range);

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    VK_CALL_NO_VALUE(cmd.begin(beginInfo));
    {
        cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                            vk::PipelineStageFlagBits::eTransfer,
                            vk::DependencyFlagBits::eByRegion, {}, {},
                            {barrier});
        cmd.clearColorImage(image, vk::ImageLayout::eTransferDstOptimal,
                            clearValue, range);
    }
    VK_CALL_NO_VALUE(cmd.end());
}

void recordLayoutTransferCommand(vk::CommandBuffer cmd, vk::Image image) {
    vk::ImageSubresourceRange range;
    range.setBaseArrayLayer(0)
        .setBaseMipLevel(0)
        .setLayerCount(1)
        .setLevelCount(1)
        .setAspectMask(vk::ImageAspectFlagBits::eColor);

    vk::ImageMemoryBarrier barrier;
    barrier.setImage(image)
        .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
        .setNewLayout(vk::ImageLayout::ePresentSrcKHR)
        .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setSubresourceRange(range);

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    VK_CALL_NO_VALUE(cmd.begin(beginInfo));
    {
        cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                            vk::PipelineStageFlagBits::eColorAttachmentOutput,
                            vk::DependencyFlagBits::eByRegion, {}, {},
                            {barrier});
    }
    VK_CALL_NO_VALUE(cmd.end());
}

void UpdateSystem(gecs::resource<gecs::mut<nickel::vulkan::Device>> device,
                  gecs::resource<gecs::mut<Context>> ctx) {
    auto index =
        device->GetSwapchain().AcquireNextImage({}, nullptr, ctx->fence);

    auto cmds = ctx->cmdPool->Allocate(vk::CommandBufferLevel::ePrimary, 2);

    recordCommand(cmds[0], device->GetSwapchain().Images()[index]);
    recordLayoutTransferCommand(cmds[1], device->GetSwapchain().Images()[index]);

    auto queue = device->GetGraphicsQueue();
    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(cmds);
    VK_CALL_NO_VALUE(queue.submit(submitInfo));

    vk::PresentInfoKHR presentInfo;
    std::array<vk::SwapchainKHR, 1> swapchains{device->GetSwapchain()};
    presentInfo.setImageIndices(index).setSwapchains(swapchains);
    VK_CALL_NO_VALUE(device->GetPresentQueue().presentKHR(presentInfo));

    ctx->fence->Wait({});
    ctx->fence->Reset();

    VK_CALL_NO_VALUE(device->GetDevice().waitIdle());

    ctx->cmdPool->Reset();
}

void ShutdownSystem(gecs::commands cmds) {
    cmds.remove_resource<nickel::vulkan::Device>();
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    nickel::Window& window =
        reg.commands().emplace_resource<nickel::Window>("01 clear", 1024, 720);

    reg
        // startup systems
        .regist_startup_system<nickel::VideoSystemInit>()
        .regist_startup_system<nickel::EventPollerInit>()
        .regist_startup_system<nickel::InputSystemInit>()
        .regist_startup_system<StartupSystem>()
        // shutdown systems
        .regist_shutdown_system<ShutdownSystem>()
        .regist_shutdown_system<nickel::EngineShutdown>()
        // update systems
        .regist_update_system<nickel::VideoSystemUpdate>()
        // other input handle event must put here(after mouse/keyboard update)
        .regist_update_system<nickel::Mouse::Update>()
        .regist_update_system<nickel::Keyboard::Update>()
        .regist_update_system<nickel::HandleInputEvents>()
        .regist_update_system<UpdateSystem>();
}