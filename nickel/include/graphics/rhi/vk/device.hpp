#pragma once

#include "graphics/rhi/impl/device.hpp"
#include "graphics/rhi/queue.hpp"
#include "graphics/rhi/render_pipeline.hpp"
#include "graphics/rhi/renderpass.hpp"
#include "graphics/rhi/vk/adapter.hpp"
#include "graphics/rhi/vk/command.hpp"
#include "graphics/rhi/vk/framebuffer.hpp"
#include "graphics/rhi/vk/pch.hpp"
#include "graphics/rhi/vk/renderpass.hpp"
#include "graphics/rhi/vk/swapchain.hpp"


namespace nickel::rhi::vulkan {

class AdapterImpl;

class DeviceImpl : public rhi::DeviceImpl {
public:
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsIndex;
        std::optional<uint32_t> presentIndex;

        explicit operator bool() const { return graphicsIndex && presentIndex; }

        std::set<uint32_t> GetUniqueIndices() const {
            return {graphicsIndex.value(), presentIndex.value()};
        }

        bool HasSeperateQueue() const {
            return graphicsIndex.value() != presentIndex.value();
        }
    };

    explicit DeviceImpl(AdapterImpl&);
    ~DeviceImpl();

    Texture CreateTexture(const Texture::Descriptor& desc) override;
    RenderPipeline CreateRenderPipeline(
        const RenderPipeline::Descriptor&) override;
    PipelineLayout CreatePipelineLayout(
        const PipelineLayout::Descriptor&) override;
    Sampler CreateSampler(const Sampler::Descriptor&) override;
    CommandEncoder CreateCommandEncoder() override;
    BindGroup CreateBindGroup(const BindGroup::Descriptor& desc) override;
    BindGroupLayout CreateBindGroupLayout(
        const BindGroupLayout::Descriptor& desc) override;
    ShaderModule CreateShaderModule(const ShaderModule::Descriptor&) override;
    Buffer CreateBuffer(const Buffer::Descriptor&) override;

    void OnWindowResize(int x, int y) override;

    std::pair<Texture, TextureView> GetPresentationTexture() override;
    void BeginFrame() override;
    void EndFrame() override;
    void WaitIdle() override;

    Queue GetQueue() override;

    vk::CommandBuffer RequireCmdBuf();
    void ResetCmdBuf(vk::CommandBuffer cmdBuf);

    AdapterImpl& adapter;
    vk::Device device;
    vk::CommandPool cmdPool;
    QueueFamilyIndices queueIndices;
    Swapchain swapchain;
    Queue* graphicsQueue;
    Queue* presentQueue;
    uint32_t curImageIndex = 0;
    bool needPresent = false;

    uint32_t curFrame = 0;
    std::vector<vk::Fence> fences;
    std::vector<vk::Semaphore> imageAvaliableSems;
    std::vector<vk::Semaphore> renderFinishSems;

    std::vector<RenderPass> renderPasses;
    std::vector<Framebuffer> framebuffers;

    std::vector<vk::CommandBuffer> cmdBufs;
    std::vector<vk::CommandBuffer> cmdBufInVacant;

private:
    void createDevice(vk::Instance, vk::PhysicalDevice, vk::SurfaceKHR);
    QueueFamilyIndices chooseQueue(
        vk::PhysicalDevice phyDevice, vk::SurfaceKHR surface,
        const std::vector<vk::QueueFamilyProperties>& props);
    void createCmdPool();
    void createSyncObject();
};

}  // namespace nickel::rhi::vulkan