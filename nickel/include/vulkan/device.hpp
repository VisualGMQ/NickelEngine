#pragma once

#include "core/manager.hpp"
#include "pch.hpp"
#include "vertex_layout.hpp"
#include "vulkan/buffer.hpp"
#include "vulkan/command.hpp"
#include "vulkan/descriptor.hpp"
#include "vulkan/image.hpp"
#include "vulkan/pipeline.hpp"
#include "vulkan/renderpass.hpp"
#include "vulkan/shader.hpp"
#include "vulkan/swapchain.hpp"
#include "vulkan/sync.hpp"
#include "window/window.hpp"


namespace nickel::vulkan {

using ShaderModuleHandle = Handle<ShaderModule>;
using BufferHandle = Handle<Buffer>;
using ImageHandle = Handle<Image>;
using PipelineHandle = Handle<Pipeline>;
using RenderPassHandle = Handle<RenderPass>;
using CommandPoolHandle = Handle<CommandPool>;
using SemaphoreHandle = Handle<Semaphore>;
using FenceHandle = Handle<Fence>;
using EventHandle = Handle<Event>;
using FramebufferHandle = Handle<Framebuffer>;
using DescriptorPoolHandle = Handle<DescriptorPool>;
using DescriptorSetLayoutHandle = Handle<DescriptorSetLayout>;
using PipelineLayoutHandle = Handle<PipelineLayout>;

class Device {
public:
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsIndex;
        std::optional<uint32_t> presentIndex;

        explicit operator bool() const { return graphicsIndex && presentIndex; }
    };

    vk::PhysicalDevice GetPhyDevice() const { return phyDevice_; }

    vk::Device GetDevice() const { return device_; }

    auto& GetQueueFamilyIndices() const { return queueIndices_; }

    explicit Device(Window& window);
    ~Device();

    vk::Queue GetGraphicsQueue() const { return graphicsQueue_; }

    vk::Queue GetPresentQueue() const { return presentQueue_; }

    Swapchain& GetSwapchain() const { return *swapchain_; }

    ResResult<ShaderModule> CreateShaderModule(
        vk::ShaderStageFlagBits type, const std::filesystem::path& filename,
        const std::string& entry = "main");
    ResResult<Buffer> CreateBuffer(uint64_t size, vk::BufferUsageFlags usage,
                                   vk::MemoryPropertyFlags flags,
                                   std::vector<uint32_t> queueIndices);
    ResResult<Image> CreateImage(
        vk::ImageType type, vk::ImageViewType viewType,
        const vk::Extent3D& extent, vk::Format format, vk::Format viewFormat,
        vk::ImageLayout initLayout, uint32_t arrayLayer, uint32_t mipLevel,
        vk::SampleCountFlagBits sampleCount, vk::ImageUsageFlagBits usage,
        vk::ImageTiling tiling, const vk::ComponentMapping& components,
        const vk::ImageSubresourceRange& subresourceRange,
        std::vector<uint32_t> queueIndices);
    ResResult<Pipeline> CreateGraphicsPipeline(
        const VertexLayout&, const vk::PipelineInputAssemblyStateCreateInfo&,
        const std::vector<ShaderModule*>&,
        const vk::PipelineViewportStateCreateInfo&,
        const vk::PipelineRasterizationStateCreateInfo&,
        const vk::PipelineDepthStencilStateCreateInfo&,
        const vk::PipelineMultisampleStateCreateInfo&,
        const vk::PipelineColorBlendStateCreateInfo&, vk::PipelineLayout,
        vk::RenderPass);
    ResResult<RenderPass> CreateRenderPass(
        const std::vector<vk::AttachmentDescription>& attachmentDescs,
        const std::vector<vk::SubpassDescription>& subpasses,
        const std::vector<vk::SubpassDependency>& subpassDeps);
    ResResult<CommandPool> CreateCommandPool(vk::CommandPoolCreateFlags flag,
                                             uint32_t queueIndex);
    ResResult<Semaphore> CreateSemaphore();
    ResResult<Fence> CreateFence(bool signaled = false);
    ResResult<Event> CreateEvent(bool deviceOnly);
    ResResult<DescriptorSetLayout> CreateDescriptorSetLayout(
        const std::vector<vk::DescriptorSetLayoutBinding>& bindings);
    ResResult<Framebuffer> CreateFramebuffer(
        const std::vector<vk::ImageView>& views, uint32_t width,
        uint32_t height, uint32_t layers, vk::RenderPass renderPass);
    ResResult<DescriptorPool> CreateDescriptorPool(
        const std::vector<vk::DescriptorPoolSize>& sizes, uint32_t maxSetCount);
    ResResult<PipelineLayout> CreatePipelineLayout(
        const std::vector<vk::DescriptorSetLayout>& layouts,
        const std::vector<vk::PushConstantRange>& pushConstantRange);

    template <typename HandleType>
    auto Get(HandleType handle) {
        return switchManager<HandleType>().Get(handle);
    }

    template <typename HandleType>
    void Destroy(HandleType handle) {
        return switchManager<HandleType>().Destroy(handle);
    }

    template <typename HandleType>
    bool Has(HandleType handle) {
        return switchManager<HandleType>().Has(handle);
    }

private:
    vk::Instance instance_;
    vk::PhysicalDevice phyDevice_;
    vk::Device device_;
    vk::Queue graphicsQueue_;
    vk::Queue presentQueue_;
    vk::SurfaceKHR surface_;
    std::unique_ptr<Swapchain> swapchain_;
    QueueFamilyIndices queueIndices_;

    // managers
    Manager<ShaderModule> shaderModules;
    Manager<Buffer> buffers;
    Manager<Image> images;
    Manager<Pipeline> pipelines;
    Manager<RenderPass> renderPasses;
    Manager<CommandPool> cmdPools;
    Manager<Semaphore> semaphores;
    Manager<Fence> fences;
    Manager<Event> events;
    Manager<Framebuffer> framebuffers;
    Manager<DescriptorPool> descriptorPools;
    Manager<DescriptorSetLayout> descriptorSetLayouts;
    Manager<PipelineLayout> pipelineLayouts;

    void createInstance(Window&);
    void choosePhysicalDevice(const std::vector<vk::PhysicalDevice>&);
    void createDevice(vk::Instance&, vk::SurfaceKHR);
    QueueFamilyIndices chooseQueue(
        vk::PhysicalDevice phyDevice, vk::SurfaceKHR surface,
        const std::vector<vk::QueueFamilyProperties>&);

    template <typename HandleType>
    auto& switchManager() {
        using AssetType = typename HandleType::ValueType;
        if constexpr (std::is_same_v<AssetType, ShaderModule>) {
            return shaderModules;
        } else if constexpr (std::is_same_v<AssetType, Buffer>) {
            return buffers;
        } else if constexpr (std::is_same_v<AssetType, Image>) {
            return images;
        } else if constexpr (std::is_same_v<AssetType, Pipeline>) {
            return pipelines;
        } else if constexpr (std::is_same_v<AssetType, RenderPass>) {
            return renderPasses;
        } else if constexpr (std::is_same_v<AssetType, CommandPool>) {
            return cmdPools;
        } else if constexpr (std::is_same_v<AssetType, Semaphore>) {
            return semaphores;
        } else if constexpr (std::is_same_v<AssetType, Fence>) {
            return fences;
        } else if constexpr (std::is_same_v<AssetType, Event>) {
            return events;
        } else if constexpr (std::is_same_v<AssetType, Framebuffer>) {
            return framebuffers;
        } else if constexpr (std::is_same_v<AssetType, DescriptorPool>) {
            return descriptorPools;
        } else if constexpr (std::is_same_v<AssetType, DescriptorSetLayout>) {
            return descriptorSetLayouts;
        } else if constexpr (std::is_same_v<AssetType, PipelineLayout>) {
            return pipelineLayouts;
        }
    }
};

}  // namespace nickel::vulkan