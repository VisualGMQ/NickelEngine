#pragma once

#include "common/manager.hpp"
#include "vulkan/pch.hpp"
#include "stdpch.hpp"
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
#include "video/window.hpp"

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
using SamplerHandle = Handle<Sampler>;

class Device {
public:
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsIndex;
        std::optional<uint32_t> presentIndex;

        explicit operator bool() const { return graphicsIndex && presentIndex; }

        std::set<uint32_t> GetUniqueIndices() const {
            return {graphicsIndex.value(), presentIndex.value()};
        }
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
                                   const std::set<uint32_t>& queueIndices);
    ResResult<Image> CreateImage(
        vk::ImageType type, vk::ImageViewType viewType,
        const vk::Extent3D& extent, vk::TextureFormat format, vk::TextureFormat viewFormat,
        vk::ImageLayout initLayout, uint32_t arrayLayer, uint32_t mipLevel,
        vk::SampleCountFlagBits sampleCount,
        vk::Flags<vk::ImageUsageFlagBits> usage, vk::ImageTiling tiling,
        const vk::ComponentMapping& components,
        const vk::ImageSubresourceRange& subresourceRange,
        const std::set<uint32_t>& queueIndices);
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
    ResResult<Sampler> CreateSampler(
        vk::Filter min, vk::Filter mag, vk::SamplerMipmapMode mipmap,
        vk::SamplerAddressMode u, vk::SamplerAddressMode v,
        vk::SamplerAddressMode w, float mipLodBias, bool anisotropyEnable,
        float maxAnisotropy, bool compareEnable, vk::CompareOp compare,
        float minLod, float maxLod, vk::BorderColor borderColor,
        bool unnormalizedCoordinates);

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
    Manager<ShaderModule> shaderModules_;
    Manager<Buffer> buffers_;
    Manager<Image> images_;
    Manager<Pipeline> pipelines_;
    Manager<RenderPass> renderPasses_;
    Manager<CommandPool> cmdPools_;
    Manager<Semaphore> semaphores_;
    Manager<Fence> fences_;
    Manager<Event> events_;
    Manager<Framebuffer> framebuffers_;
    Manager<DescriptorPool> descriptorPools_;
    Manager<DescriptorSetLayout> descriptorSetLayouts_;
    Manager<PipelineLayout> pipelineLayouts_;
    Manager<Sampler> samplers_;

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
            return shaderModules_;
        } else if constexpr (std::is_same_v<AssetType, Buffer>) {
            return buffers_;
        } else if constexpr (std::is_same_v<AssetType, Image>) {
            return images_;
        } else if constexpr (std::is_same_v<AssetType, Pipeline>) {
            return pipelines_;
        } else if constexpr (std::is_same_v<AssetType, RenderPass>) {
            return renderPasses_;
        } else if constexpr (std::is_same_v<AssetType, CommandPool>) {
            return cmdPools_;
        } else if constexpr (std::is_same_v<AssetType, Semaphore>) {
            return semaphores_;
        } else if constexpr (std::is_same_v<AssetType, Fence>) {
            return fences_;
        } else if constexpr (std::is_same_v<AssetType, Event>) {
            return events_;
        } else if constexpr (std::is_same_v<AssetType, Framebuffer>) {
            return framebuffers_;
        } else if constexpr (std::is_same_v<AssetType, DescriptorPool>) {
            return descriptorPools_;
        } else if constexpr (std::is_same_v<AssetType, DescriptorSetLayout>) {
            return descriptorSetLayouts_;
        } else if constexpr (std::is_same_v<AssetType, PipelineLayout>) {
            return pipelineLayouts_;
        } else if constexpr (std::is_same_v<AssetType, Sampler>) {
            return samplers_;
        }
    }
};

}  // namespace nickel::vulkan