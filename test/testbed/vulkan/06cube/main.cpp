#include "nickel.hpp"
#include "vulkan/device.hpp"
#include "vulkan/util.hpp"
#include "stb_image.h"

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
    nickel::vulkan::Image* image{};
    nickel::vulkan::Sampler* sampler{};
    nickel::vulkan::Image* depth{};
};

struct Vertex {
    nickel::cgmath::Vec3 position;
    nickel::cgmath::Vec2 texcoord;
};

struct MVP {
    nickel::cgmath::Mat44 proj, view;  // for uniform
    nickel::cgmath::Mat44 model;       // for push constants
} mvp;

std::array<Vertex, 36> vertices = {
    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f, -0.5f},  nickel::cgmath::Vec2{0.0f, 0.0f}}, // Bottom-left
    Vertex{nickel::cgmath::Vec3{ 0.5f,  0.5f, -0.5f},  nickel::cgmath::Vec2{1.0f, 1.0f}}, // top-right
    Vertex{nickel::cgmath::Vec3{ 0.5f, -0.5f, -0.5f},  nickel::cgmath::Vec2{1.0f, 0.0f}}, // bottom-right         
    Vertex{nickel::cgmath::Vec3{ 0.5f,  0.5f, -0.5f},  nickel::cgmath::Vec2{1.0f, 1.0f}}, // top-right
    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f, -0.5f},  nickel::cgmath::Vec2{0.0f, 0.0f}}, // bottom-left
    Vertex{nickel::cgmath::Vec3{-0.5f,  0.5f, -0.5f},  nickel::cgmath::Vec2{0.0f, 1.0f}}, // top-left

    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f,  0.5f},  nickel::cgmath::Vec2{0.0f, 0.0f}}, // bottom-left
    Vertex{nickel::cgmath::Vec3{ 0.5f, -0.5f,  0.5f},  nickel::cgmath::Vec2{1.0f, 0.0f}}, // bottom-right
    Vertex{nickel::cgmath::Vec3{ 0.5f,  0.5f,  0.5f},  nickel::cgmath::Vec2{1.0f, 1.0f}}, // top-right
    Vertex{nickel::cgmath::Vec3{ 0.5f,  0.5f,  0.5f},  nickel::cgmath::Vec2{1.0f, 1.0f}}, // top-right
    Vertex{nickel::cgmath::Vec3{-0.5f,  0.5f,  0.5f},  nickel::cgmath::Vec2{0.0f, 1.0f}}, // top-left
    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f,  0.5f},  nickel::cgmath::Vec2{0.0f, 0.0f}}, // bottom-left

    Vertex{nickel::cgmath::Vec3{-0.5f,  0.5f,  0.5f},  nickel::cgmath::Vec2{1.0f, 0.0f}}, // top-right
    Vertex{nickel::cgmath::Vec3{-0.5f,  0.5f, -0.5f},  nickel::cgmath::Vec2{1.0f, 1.0f}}, // top-left
    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f, -0.5f},  nickel::cgmath::Vec2{0.0f, 1.0f}}, // bottom-left
    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f, -0.5f},  nickel::cgmath::Vec2{0.0f, 1.0f}}, // bottom-left
    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f,  0.5f},  nickel::cgmath::Vec2{0.0f, 0.0f}}, // bottom-right
    Vertex{nickel::cgmath::Vec3{-0.5f,  0.5f,  0.5f},  nickel::cgmath::Vec2{1.0f, 0.0f}}, // top-right

    Vertex{nickel::cgmath::Vec3{ 0.5f,  0.5f,  0.5f},  nickel::cgmath::Vec2{1.0f, 0.0f}}, // top-left
    Vertex{nickel::cgmath::Vec3{ 0.5f, -0.5f, -0.5f},  nickel::cgmath::Vec2{0.0f, 1.0f}}, // bottom-right
    Vertex{nickel::cgmath::Vec3{ 0.5f,  0.5f, -0.5f},  nickel::cgmath::Vec2{1.0f, 1.0f}}, // top-right         
    Vertex{nickel::cgmath::Vec3{ 0.5f, -0.5f, -0.5f},  nickel::cgmath::Vec2{0.0f, 1.0f}}, // bottom-right
    Vertex{nickel::cgmath::Vec3{ 0.5f,  0.5f,  0.5f},  nickel::cgmath::Vec2{1.0f, 0.0f}}, // top-left
    Vertex{nickel::cgmath::Vec3{ 0.5f, -0.5f,  0.5f},  nickel::cgmath::Vec2{0.0f, 0.0f}}, // bottom-left     

    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f, -0.5f},  nickel::cgmath::Vec2{0.0f, 1.0f}}, // top-right
    Vertex{nickel::cgmath::Vec3{ 0.5f, -0.5f, -0.5f},  nickel::cgmath::Vec2{1.0f, 1.0f}}, // top-left
    Vertex{nickel::cgmath::Vec3{ 0.5f, -0.5f,  0.5f},  nickel::cgmath::Vec2{1.0f, 0.0f}}, // bottom-left
    Vertex{nickel::cgmath::Vec3{ 0.5f, -0.5f,  0.5f},  nickel::cgmath::Vec2{1.0f, 0.0f}}, // bottom-left
    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f,  0.5f},  nickel::cgmath::Vec2{0.0f, 0.0f}}, // bottom-right
    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f, -0.5f},  nickel::cgmath::Vec2{0.0f, 1.0f}}, // top-right

    Vertex{nickel::cgmath::Vec3{-0.5f,  0.5f, -0.5f},  nickel::cgmath::Vec2{0.0f, 1.0f}}, // top-left
    Vertex{nickel::cgmath::Vec3{ 0.5f,  0.5f,  0.5f},  nickel::cgmath::Vec2{1.0f, 0.0f}}, // bottom-right
    Vertex{nickel::cgmath::Vec3{ 0.5f,  0.5f, -0.5f},  nickel::cgmath::Vec2{1.0f, 1.0f}}, // top-right     
    Vertex{nickel::cgmath::Vec3{ 0.5f,  0.5f,  0.5f},  nickel::cgmath::Vec2{1.0f, 0.0f}}, // bottom-right
    Vertex{nickel::cgmath::Vec3{-0.5f,  0.5f, -0.5f},  nickel::cgmath::Vec2{0.0f, 1.0f}}, // top-left
    Vertex{nickel::cgmath::Vec3{-0.5f,  0.5f,  0.5f},  nickel::cgmath::Vec2{0.0f, 0.0f}}  // bottom-left    
};

auto createPipelineLayout(const nickel::vulkan::PipelineLayoutDescription& desc,
                          nickel::vulkan::Device& device, Context& ctx) {
    return device.CreatePipelineLayout({*ctx.setLayout},
                                       {desc.GetPushConstsRange()});
}

auto createRenderPass(nickel::vulkan::Device& device) {
    auto& swapchain = device.GetSwapchain();

    vk::AttachmentDescription colorAttach;
    colorAttach.setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFormat(swapchain.ImageInfo().format.format)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eNone)
        .setSamples(vk::SampleCountFlagBits::e1);

    vk::AttachmentDescription depthAttach;
    depthAttach.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFormat(vk::Format::eD24UnormS8Uint)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setSamples(vk::SampleCountFlagBits::e1);

    vk::SubpassDescription subpass;
    std::array<vk::AttachmentReference, 2> refs;
    refs[0]
        .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setAttachment(0);
    refs[1]
        .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .setAttachment(1);
    subpass.setColorAttachments(refs[0]).setPipelineBindPoint(
        vk::PipelineBindPoint::eGraphics)
        .setPDepthStencilAttachment(&refs[1]);

    vk::SubpassDependency dep;
    dep.setSrcSubpass(VK_SUBPASS_EXTERNAL)
        .setDstSubpass(0)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput |
                         vk::PipelineStageFlagBits::eEarlyFragmentTests)
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput |
                         vk::PipelineStageFlagBits::eEarlyFragmentTests)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite |
                          vk::AccessFlagBits::eDepthStencilAttachmentWrite);

    return device.CreateRenderPass({colorAttach, depthAttach}, {subpass},
                                   {dep});
}

auto createPipeline(nickel::vulkan::Device& device,
                    const nickel::vulkan::PipelineLayout& layout,
                    const nickel::vulkan::RenderPass& renderPass) {
    auto vertexLayout = nickel::vulkan::VertexLayout::CreateFromTypes(
        {nickel::vulkan::ShaderDataType::Vec3,
         nickel::vulkan::ShaderDataType::Vec2});

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
                                "test/testbed/vulkan/06cube/vert.spv")
            .value;
    auto fragShader =
        device
            .CreateShaderModule(vk::ShaderStageFlagBits::eFragment,
                                "test/testbed/vulkan/06cube/frag.spv")
            .value;

    vk::PipelineRasterizationStateCreateInfo raster;
    raster.setLineWidth(1)
        .setRasterizerDiscardEnable(false)
        .setCullMode(vk::CullModeFlagBits::eNone)
        .setCullMode(vk::CullModeFlagBits::eBack)
        .setPolygonMode(vk::PolygonMode::eFill);

    vk::PipelineColorBlendStateCreateInfo colorBlend;
    vk::PipelineColorBlendAttachmentState blendAttach;
    blendAttach.setBlendEnable(false).setColorWriteMask(
        vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB);
    colorBlend.setLogicOpEnable(false).setAttachments(blendAttach);

    vk::PipelineDepthStencilStateCreateInfo depthStencilState;
    depthStencilState.setDepthTestEnable(true)
        .setStencilTestEnable(false)
        .setDepthWriteEnable(true)
        .setDepthCompareOp(vk::CompareOp::eGreater);

    return device.CreateGraphicsPipeline(
        vertexLayout, inputAsm, {vertShader, fragShader}, viewportState, raster,
        depthStencilState, {}, colorBlend, layout, renderPass);
}

auto createBuffer(nickel::vulkan::Device& device) {
    return device
        .CreateBuffer(sizeof(vertices), vk::BufferUsageFlagBits::eVertexBuffer,
                      vk::MemoryPropertyFlagBits::eHostVisible |
                          vk::MemoryPropertyFlagBits::eHostCoherent,
                      {device.GetQueueFamilyIndices().graphicsIndex.value()})
        .value;
}

nickel::vulkan::Image* createImage(nickel::vulkan::Device& device,
                                   nickel::vulkan::CommandPool& cmdPool) {
    int w, h;
    auto imgData = stbi_load("test/testbed/vulkan/05image/texture.jpg", &w, &h,
                             nullptr, STBI_rgb_alpha);

    auto image =
        device
            .CreateImage(vk::ImageType::e2D, vk::ImageViewType::e2D,
                         {(uint32_t)w, (uint32_t)h, 1},
                         vk::Format::eR8G8B8A8Srgb, vk::Format::eR8G8B8A8Srgb,
                         vk::ImageLayout::eUndefined, 1, 1,
                         vk::SampleCountFlagBits::e1,
                         vk::ImageUsageFlagBits::eSampled |
                             vk::ImageUsageFlagBits::eTransferDst,
                         vk::ImageTiling::eLinear, {},
                         vk::ImageSubresourceRange{
                             vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1},
                         {device.GetQueueFamilyIndices().graphicsIndex.value()})
            .value;

    if (!image) {
        return {};
    }

    auto buffer =
        device
            .CreateBuffer(
                4 * w * h, vk::BufferUsageFlagBits::eTransferSrc,
                vk::MemoryPropertyFlagBits::eHostCoherent |
                    vk::MemoryPropertyFlagBits::eHostVisible,
                {device.GetQueueFamilyIndices().graphicsIndex.value()})
            .value;

    void* map = buffer->Map(0, buffer->Size());
    memcpy(map, imgData, buffer->Size());
    buffer->Unmap();

    auto cmd = cmdPool.Allocate(vk::CommandBufferLevel::ePrimary, 1)[0];
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    VK_CALL_NO_VALUE(cmd.begin(beginInfo));
    {
        vk::ImageSubresourceRange range;
        range.setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setBaseArrayLayer(0)
            .setLevelCount(1)
            .setLayerCount(1)
            .setBaseMipLevel(0);
        vk::ImageMemoryBarrier barrier;
        barrier.setImage(image->GetImage())
            .setOldLayout(vk::ImageLayout::eUndefined)
            .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
            .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
            .setSrcQueueFamilyIndex(
                device.GetQueueFamilyIndices().graphicsIndex.value())
            .setSubresourceRange(range);
        cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                            vk::PipelineStageFlagBits::eTransfer,
                            vk::DependencyFlagBits::eByRegion, {}, {}, barrier);

        vk::ImageSubresourceLayers layers;
        layers.setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setBaseArrayLayer(0)
            .setLayerCount(1)
            .setMipLevel(0);
        vk::BufferImageCopy copyInfo;
        copyInfo.setBufferOffset(0)
            .setImageOffset(0)
            .setBufferImageHeight(0)
            .setBufferRowLength(0)
            .setImageExtent(vk::Extent3D(w, h, 1))
            .setImageSubresource(layers);
        cmd.copyBufferToImage(buffer->GetBuffer(), image->GetImage(),
                              vk::ImageLayout::eTransferDstOptimal, copyInfo);

        barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
            .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
            .setDstAccessMask(vk::AccessFlagBits::eShaderRead);
        cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                            vk::PipelineStageFlagBits::eFragmentShader,
                            vk::DependencyFlagBits::eByRegion, {}, {}, barrier);
    }
    VK_CALL_NO_VALUE(cmd.end());

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(cmd);
    VK_CALL_NO_VALUE(device.GetGraphicsQueue().submit(submitInfo));
    VK_CALL_NO_VALUE(device.GetDevice().waitIdle());

    return image;
}

nickel::vulkan::Image* createDepthImage(nickel::vulkan::Device& device,
                                        nickel::vulkan::CommandPool& cmdPool,
                                        vk::Extent2D extent) {
    auto image =
        device
            .CreateImage(
                vk::ImageType::e2D, vk::ImageViewType::e2D,
                {extent.width, extent.height, 1},
                vk::Format::eD24UnormS8Uint, vk::Format::eD24UnormS8Uint,
                vk::ImageLayout::eUndefined, 1, 1, vk::SampleCountFlagBits::e1,
                vk::ImageUsageFlagBits::eDepthStencilAttachment,
                vk::ImageTiling::eOptimal, {},
                vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eDepth |
                                              vk::ImageAspectFlagBits::eStencil,
                                          0, 1, 0, 1},
                {device.GetQueueFamilyIndices().graphicsIndex.value()})
            .value;

    if (!image) {
        return {};
    }

    auto cmd = cmdPool.Allocate(vk::CommandBufferLevel::ePrimary, 1)[0];
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    VK_CALL_NO_VALUE(cmd.begin(beginInfo));
    {
        vk::ImageSubresourceRange range;
        range
            .setAspectMask(vk::ImageAspectFlagBits::eDepth |
                           vk::ImageAspectFlagBits::eStencil)
            .setBaseArrayLayer(0)
            .setLevelCount(1)
            .setLayerCount(1)
            .setBaseMipLevel(0);
        vk::ImageMemoryBarrier barrier;
        barrier.setImage(image->GetImage())
            .setOldLayout(vk::ImageLayout::eUndefined)
            .setNewLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
            .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead |
                              vk::AccessFlagBits::eDepthStencilAttachmentWrite)
            .setSrcQueueFamilyIndex(
                device.GetQueueFamilyIndices().graphicsIndex.value())
            .setSubresourceRange(range);
        cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                            vk::PipelineStageFlagBits::eEarlyFragmentTests,
                            vk::DependencyFlagBits::eByRegion, {}, {}, barrier);
    }
    VK_CALL_NO_VALUE(cmd.end());

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(cmd);
    VK_CALL_NO_VALUE(device.GetGraphicsQueue().submit(submitInfo));
    VK_CALL_NO_VALUE(device.GetDevice().waitIdle());

    return image;
}

nickel::vulkan::Sampler* createSampler(nickel::vulkan::Device& device) {
    return device
        .CreateSampler(
            vk::Filter::eLinear, vk::Filter::eLinear,
            vk::SamplerMipmapMode::eLinear, vk::SamplerAddressMode::eRepeat,
            vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat,
            1.0, false, 1.0, false, vk::CompareOp::eAlways, 1.0, 1.0,
            vk::BorderColor::eIntOpaqueWhite, false)
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

void createDescriptors(const nickel::vulkan::PipelineLayoutDescription& desc,
                       nickel::vulkan::Device& device, Context& ctx) {
    ctx.setLayout =
        device.CreateDescriptorSetLayout(desc.GetLayoutBindings()).value;

    auto imageSize = device.GetSwapchain().ImageInfo().imagCount;

    auto sizes = desc.GetSizes(imageSize);
    ctx.descriptorPool = device.CreateDescriptorPool(sizes, imageSize).value;

    ctx.descriptorSets = ctx.descriptorPool->AllocSet(
        imageSize, {*ctx.setLayout, *ctx.setLayout});
}

void bindDescriptorSetAndBuffer(nickel::vulkan::Device& device,
                                nickel::vulkan::Buffer& buffer,
                                const nickel::vulkan::Image& image,
                                const nickel::vulkan::Sampler& sampler,
                                const std::vector<vk::DescriptorSet>& sets) {
    for (auto set : sets) {
        vk::DescriptorBufferInfo bufferInfo;
        bufferInfo.setBuffer(buffer.GetBuffer())
            .setOffset(0)
            .setRange(buffer.Size());
        vk::WriteDescriptorSet bufferSet;
        bufferSet.setBufferInfo(bufferInfo)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDstBinding(0)
            .setDstArrayElement(0)
            .setDstSet(set);

        vk::DescriptorImageInfo imageInfo;
        imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setImageView(image.GetView())
            .setSampler(sampler);
        vk::WriteDescriptorSet imageSet;
        imageSet.setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
            .setDstBinding(1)
            .setDstArrayElement(0)
            .setDstSet(set)
            .setImageInfo(imageInfo);

        device.GetDevice().updateDescriptorSets({bufferSet, imageSet}, {});
    }
}

void RenderInitSystem(gecs::commands cmds,
                      gecs::resource<gecs::mut<nickel::Window>> window) {
    nickel::vulkan::PipelineLayoutDescription desc;
    desc.AddBinding(0, {
                           vk::DescriptorType::eUniformBuffer,
                           vk::ShaderStageFlagBits::eVertex,
                           {nickel::vulkan::ShaderDataType::Mat4,
                                                           nickel::vulkan::ShaderDataType::Mat4}
    });
    desc.AddBinding(1, {vk::DescriptorType::eCombinedImageSampler,
                        vk::ShaderStageFlagBits::eFragment,
                        {}});
    desc.SetPushConstant(0, 4 * 4 * 4, vk::ShaderStageFlagBits::eVertex);

    auto& ctx = cmds.emplace_resource<Context>();
    auto& device = cmds.emplace_resource<nickel::vulkan::Device>(window.get());
    createDescriptors(desc, device, ctx);
    ctx.cmdPool = device
                      .CreateCommandPool(
                          vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                          device.GetQueueFamilyIndices().graphicsIndex.value())
                      .value;
    ctx.depth = createDepthImage(device, *ctx.cmdPool, device.GetSwapchain().ImageInfo().extent);
    ctx.pipelineLayout = createPipelineLayout(desc, device, ctx).value;
    ctx.renderPass = createRenderPass(device).value;
    ctx.pipeline =
        createPipeline(device, *ctx.pipelineLayout, *ctx.renderPass).value;

    auto& swapchain = device.GetSwapchain();
    for (auto view : swapchain.ImageViews()) {
        auto fbo = device
                       .CreateFramebuffer({view, ctx.depth->GetView()},
                                          swapchain.ImageInfo().extent.width,
                                          swapchain.ImageInfo().extent.height,
                                          1, *ctx.renderPass)
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

    ctx.image = createImage(device, *ctx.cmdPool);
    ctx.sampler = createSampler(device);

    bindDescriptorSetAndBuffer(device, *ctx.uniformBuf, *ctx.image,
                               *ctx.sampler, ctx.descriptorSets);

    void* map = ctx.buffer->Map(0, ctx.buffer->Size());
    memcpy(map, vertices.data(), sizeof(vertices));
    ctx.buffer->Unmap();

    mvp.proj = nickel::cgmath::CreatePersp(nickel::cgmath::Deg2Rad(45.0f),
                                           window->Size().w / window->Size().h,
                                           0.1, 100);
    mvp.view = nickel::cgmath::Mat44::Identity();
    mvp.model =
        nickel::cgmath::CreateTranslation(nickel::cgmath::Vec3{0, 0, 3});

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
        std::array<vk::ClearValue, 2> clearValues = {
            vk::ClearColorValue{0.3f, 0.3f, 0.3f, 1.0f},
            vk::ClearDepthStencilValue{0.0f, 0}
        };
        vk::RenderPassBeginInfo renderPassBeginInfo;
        renderPassBeginInfo.setRenderPass(*ctx.renderPass)
            .setRenderArea(
                vk::Rect2D{{}, device.GetSwapchain().ImageInfo().extent})
            .setFramebuffer(fbo)
            .setClearValues(clearValues);
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

            cmd.draw(vertices.size(), 1, 0, 0);
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
    nickel::cgmath::Vec3 offset;

    if (keyboard->Key(nickel::Key::A).IsPress()) {
        offset.x -= 0.001;
    }

    if (keyboard->Key(nickel::Key::D).IsPress()) {
        offset.x += 0.001;
    }

    if (keyboard->Key(nickel::Key::W).IsPress()) {
        offset.y += 0.001;
    }

    if (keyboard->Key(nickel::Key::S).IsPress()) {
        offset.y -= 0.001;
    }

    if (keyboard->Key(nickel::Key::I).IsPress()) {
        offset.z -= 0.001;
    }

    if (keyboard->Key(nickel::Key::K).IsPress()) {
        offset.z += 0.001;
    }

    static nickel::cgmath::Vec3 pos{0, 0, -3};
    pos += offset;

    static float rotationY = 0;
    rotationY += 0.001;
    static float rotationX = 0;
    rotationX += 0.003;
    mvp.model = nickel::cgmath::CreateTranslation(pos) *
                nickel::cgmath::CreateXYZRotation({rotationX, rotationY, 0});
}

void ShutdownSystem(gecs::commands cmds) {
    cmds.remove_resource<nickel::vulkan::Device>();
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    nickel::Window& window =
        reg.commands().emplace_resource<nickel::Window>("06 cube", 1024, 720);

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