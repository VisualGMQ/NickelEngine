#include "nickel.hpp"
#include "vulkan/device.hpp"
#include "vulkan/util.hpp"

struct Context {
    nickel::vulkan::Pipeline* pipeline{};
    nickel::vulkan::RenderPass* renderPass{};
    nickel::vulkan::CommandPool* cmdPool{};
    std::vector<nickel::vulkan::Framebuffer*> fbos;
    std::vector<nickel::vulkan::Semaphore*> waitImageAvaliableSemaphore;
    std::vector<nickel::vulkan::Semaphore*> graphicsFinishSemaphore;
    std::vector<nickel::vulkan::Fence*> fences;
    std::vector<vk::CommandBuffer> cmds;
    int frameIndex = 0;

    nickel::vulkan::DescriptorPool* descriptorPool{};
    std::vector<vk::DescriptorSet> descriptorSets;
    nickel::vulkan::DescriptorSetLayout* setLayout{};
    nickel::vulkan::PipelineLayout* pipelineLayout{};

    nickel::vulkan::Buffer* buffer{};
    nickel::vulkan::Buffer* uniformBuf{};
};

struct Vertex {
    nickel::cgmath::Vec2 position;
    nickel::cgmath::Vec3 color;
};

struct MVP {
    nickel::cgmath::Mat44 proj, view;  // for uniform
    nickel::cgmath::Mat44 model;       // for push constants
} mvp;

std::array<Vertex, 3> vertices = {
    Vertex{nickel::cgmath::Vec2{0.0f, -0.5f},
           nickel::cgmath::Vec3{1.0f, 0.0f, 0.0f}},
    Vertex{ nickel::cgmath::Vec2{0.5f, 0.5f},
           nickel::cgmath::Vec3{0.0f, 1.0f, 0.0f}},
    Vertex{nickel::cgmath::Vec2{-0.5f, 0.5f},
           nickel::cgmath::Vec3{0.0f, 0.0f, 1.0f}}
};

auto createPipelineLayout(nickel::vulkan::Device& device, Context& ctx) {
    vk::PushConstantRange range;
    range.setOffset(0).setSize(4 * 4 * 4).setStageFlags(
        vk::ShaderStageFlagBits::eVertex);
    return device.CreatePipelineLayout({*ctx.setLayout}, {range});
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
    auto vertexLayout = nickel::vulkan::VertexLayout::CreateFromTypes(
        {nickel::vulkan::Attribute::Type::Vec2,
         nickel::vulkan::Attribute::Type::Vec3});

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
                                "test/testbed/vulkan/04uniform/vert.spv")
            .value;
    auto fragShader =
        device
            .CreateShaderModule(vk::ShaderStageFlagBits::eFragment,
                                "test/testbed/vulkan/04uniform/frag.spv")
            .value;

    vk::PipelineRasterizationStateCreateInfo raster;
    raster.setLineWidth(1)
        .setRasterizerDiscardEnable(false)
        .setCullMode(vk::CullModeFlagBits::eNone)
        .setPolygonMode(vk::PolygonMode::eFill);

    vk::PipelineColorBlendStateCreateInfo colorBlend;
    vk::PipelineColorBlendAttachmentState blendAttach;
    blendAttach.setBlendEnable(false).setColorWriteMask(
        vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB);
    colorBlend.setLogicOpEnable(false).setAttachments(blendAttach);

    return device.CreateGraphicsPipeline(
        vertexLayout, inputAsm, {vertShader, fragShader}, viewportState, raster,
        {}, {}, colorBlend, layout, renderPass);
}

auto createBuffer(nickel::vulkan::Device& device) {
    return device
        .CreateBuffer(4 * 5 * 3, vk::BufferUsageFlagBits::eVertexBuffer,
                      vk::MemoryPropertyFlagBits::eHostVisible |
                          vk::MemoryPropertyFlagBits::eHostCoherent,
                      {device.GetQueueFamilyIndices().graphicsIndex.value()})
        .value;
}

auto createUniformBuffer(nickel::vulkan::Device& device) {
    return device
        .CreateBuffer(4 * 4 * 4 * 2, vk::BufferUsageFlagBits::eUniformBuffer,
                      vk::MemoryPropertyFlagBits::eHostVisible |
                          vk::MemoryPropertyFlagBits::eHostCoherent,
                      {device.GetQueueFamilyIndices().graphicsIndex.value()})
        .value;
}

void createDescriptors(nickel::vulkan::Device& device, Context& ctx) {
    vk::DescriptorSetLayoutBinding binding;
    binding.setBinding(0)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setStageFlags(vk::ShaderStageFlagBits::eVertex);
    ctx.setLayout = device.CreateDescriptorSetLayout({binding}).value;

    auto imageSize = device.GetSwapchain().ImageInfo().imagCount;

    vk::DescriptorPoolSize size;
    size.setType(vk::DescriptorType::eUniformBuffer)
        .setDescriptorCount(imageSize);
    ctx.descriptorPool = device.CreateDescriptorPool({size}, imageSize).value;

    ctx.descriptorSets = ctx.descriptorPool->AllocSet(
        imageSize, {*ctx.setLayout, *ctx.setLayout});
}

void bindDescriptorSetAndBuffer(nickel::vulkan::Device& device,
                                nickel::vulkan::Buffer& buffer,
                                const std::vector<vk::DescriptorSet>& sets) {
    for (auto set : sets) {
        vk::WriteDescriptorSet writeInfo;
        vk::DescriptorBufferInfo bufferInfo;
        bufferInfo.setBuffer(buffer.GetBuffer())
            .setOffset(0)
            .setRange(buffer.Size());
        writeInfo.setBufferInfo(bufferInfo)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDstBinding(0)
            .setDstArrayElement(0)
            .setDstSet(set);
        device.GetDevice().updateDescriptorSets(writeInfo, {});
    }
}

void RenderInitSystem(gecs::commands cmds,
                      gecs::resource<gecs::mut<nickel::Window>> window) {
    auto& ctx = cmds.emplace_resource<Context>();
    auto& device = cmds.emplace_resource<nickel::vulkan::Device>(window.get());
    createDescriptors(device, ctx);
    ctx.pipelineLayout = createPipelineLayout(device, ctx).value;
    ctx.renderPass = createRenderPass(device).value;
    ctx.pipeline =
        createPipeline(device, *ctx.pipelineLayout, *ctx.renderPass).value;
    ctx.cmdPool = device
                      .CreateCommandPool(
                          vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
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

        ctx.graphicsFinishSemaphore.emplace_back(
            device.CreateSemaphore().value);
        ctx.waitImageAvaliableSemaphore.emplace_back(
            device.CreateSemaphore().value);
        ctx.cmds.emplace_back(
            ctx.cmdPool->Allocate(vk::CommandBufferLevel::ePrimary, 1)[0]);
        ctx.fences.emplace_back(device.CreateFence().value);
    }

    ctx.buffer = createBuffer(device);
    ctx.uniformBuf = createUniformBuffer(device);

    bindDescriptorSetAndBuffer(device, *ctx.uniformBuf, ctx.descriptorSets);

    static std::array<Vertex, 3> vertices = {
        Vertex{nickel::cgmath::Vec2{0.0f, -0.5f},
               nickel::cgmath::Vec3{1.0f, 0.0f, 0.0f}},
        Vertex{ nickel::cgmath::Vec2{0.5f, 0.5f},
               nickel::cgmath::Vec3{0.0f, 1.0f, 0.0f}},
        Vertex{nickel::cgmath::Vec2{-0.5f, 0.5f},
               nickel::cgmath::Vec3{0.0f, 0.0f, 1.0f}}
    };

    void* map = ctx.buffer->Map(0, ctx.buffer->Size());
    memcpy(map, vertices.data(), sizeof(vertices));
    ctx.buffer->Unmap();

    mvp.proj = nickel::cgmath::Mat44::Identity();
    mvp.view = nickel::cgmath::Mat44::Identity();
    mvp.model = nickel::cgmath::Mat44::Identity();

    map = ctx.uniformBuf->Map(0, ctx.uniformBuf->Size());
    memcpy(map, &mvp, ctx.uniformBuf->Size());
    ctx.uniformBuf->Unmap();
}

void recordCommand(nickel::vulkan::Device& device, Context& ctx,
                   vk::CommandBuffer cmd, vk::Framebuffer fbo) {
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    VK_CALL_NO_VALUE(cmd.begin(beginInfo));
    {
        vk::ClearValue clearValue{
            vk::ClearColorValue{0.3f, 0.3f, 0.3f, 1.0f}
        };
        vk::RenderPassBeginInfo renderPassBeginInfo;
        renderPassBeginInfo.setRenderPass(*ctx.renderPass)
            .setRenderArea(
                vk::Rect2D{{}, device.GetSwapchain().ImageInfo().extent})
            .setFramebuffer(fbo)
            .setClearValues(clearValue);
        cmd.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
        {
            cmd.bindPipeline(vk::PipelineBindPoint::eGraphics,
                             ctx.pipeline->Raw());
            vk::Buffer buffer = ctx.buffer->GetBuffer();
            vk::DescriptorSet set = ctx.descriptorSets[ctx.frameIndex];
            cmd.bindVertexBuffers(0, buffer, {0});
            cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                   *ctx.pipelineLayout, 0, {set}, {});
            cmd.pushConstants<float>(*ctx.pipelineLayout,
                                     vk::ShaderStageFlagBits::eVertex, 0,
                                     mvp.model.data);

            cmd.draw(3, 1, 0, 0);
        }
        cmd.endRenderPass();
    }
    VK_CALL_NO_VALUE(cmd.end());
}

void RenderUpdateSystem(
    gecs::resource<gecs::mut<nickel::vulkan::Device>> device,
    gecs::resource<gecs::mut<Context>> ctx) {
    auto index = device->GetSwapchain().AcquireNextImage(
        {}, ctx->waitImageAvaliableSemaphore[ctx->frameIndex], nullptr);

    auto cmd = ctx->cmds[ctx->frameIndex];
    recordCommand(device.get(), ctx.get(), cmd, *ctx->fbos[index]);

    auto queue = device->GetGraphicsQueue();

    vk::Semaphore waitImageSem =
        *ctx->waitImageAvaliableSemaphore[ctx->frameIndex];
    vk::Semaphore graphicsFinishSem =
        *ctx->graphicsFinishSemaphore[ctx->frameIndex];
    vk::SubmitInfo submitInfo;
    vk::PipelineStageFlags dstStage =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;
    auto fence = ctx->fences[ctx->frameIndex];
    submitInfo.setCommandBuffers(cmd)
        .setWaitSemaphores(waitImageSem)
        .setWaitDstStageMask(dstStage)
        .setSignalSemaphores(graphicsFinishSem);
    VK_CALL_NO_VALUE(queue.submit(submitInfo, *fence));

    vk::PresentInfoKHR presentInfo;
    std::array<vk::SwapchainKHR, 1> swapchains{device->GetSwapchain()};
    presentInfo.setImageIndices(index)
        .setSwapchains(swapchains)
        .setWaitSemaphores(graphicsFinishSem);
    VK_CALL_NO_VALUE(device->GetPresentQueue().presentKHR(presentInfo));

    fence->Wait({});
    fence->Reset();

    cmd.reset();

    ctx->frameIndex++;
    ctx->frameIndex =
        ctx->frameIndex >= device->GetSwapchain().ImageInfo().imagCount
            ? 0
            : ctx->frameIndex;
}

void LogicUpdateSystem(gecs::resource<gecs::mut<Context>> ctx,
                       gecs::resource<nickel::Keyboard> keyboard) {
    float offset = 0;
    if (keyboard->Key(nickel::Key::A).IsPressing()) {
        offset -= 0.001;
    }

    auto& key = keyboard->Key(nickel::Key::D);
    if (key.IsPressed()) {
        offset += 0.1;
    }

    static float x = 0;
    x += offset;
    mvp.model = nickel::cgmath::CreateTranslation({x, 0, 0});
}

void ShutdownSystem(gecs::commands cmds) {
    cmds.remove_resource<nickel::vulkan::Device>();
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    nickel::Window& window = reg.commands().emplace_resource<nickel::Window>(
        "04 uniform", 1024, 720);

    reg
        // startup systems
        .regist_startup_system<nickel::VideoSystemInit>()
        .regist_startup_system<nickel::EventPollerInit>()
        .regist_startup_system<nickel::InputSystemInit>()
        .regist_startup_system<RenderInitSystem>()
        // shutdown systems
        .regist_shutdown_system<ShutdownSystem>()
        .regist_shutdown_system<nickel::EngineShutdown>()
        // update systems
        .regist_update_system<nickel::VideoSystemUpdate>()
        // other input handle event must put here(after mouse/keyboard update)
        .regist_update_system<nickel::Mouse::Update>()
        .regist_update_system<nickel::Keyboard::Update>()
        .regist_update_system<nickel::HandleInputEvents>()
        .regist_update_system<LogicUpdateSystem>()
        .regist_update_system<RenderUpdateSystem>();
}