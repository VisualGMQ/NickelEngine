#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/graphics/cmd.hpp"
#include "nickel/graphics/cmd_encoder.hpp"
#include "nickel/graphics/device.hpp"
#include "nickel/graphics/fence.hpp"
#include "nickel/graphics/internal/bind_group_layout_impl.hpp"
#include "nickel/graphics/internal/buffer_impl.hpp"
#include "nickel/graphics/internal/cmd_pool_impl.hpp"
#include "nickel/graphics/internal/fence_impl.hpp"
#include "nickel/graphics/internal/framebuffer_impl.hpp"
#include "nickel/graphics/internal/graphics_pipeline_impl.hpp"
#include "nickel/graphics/internal/image_impl.hpp"
#include "nickel/graphics/internal/image_view_impl.hpp"
#include "nickel/graphics/internal/pipeline_layout_impl.hpp"
#include "nickel/graphics/internal/render_pass_impl.hpp"
#include "nickel/graphics/internal/sampler_impl.hpp"
#include "nickel/graphics/internal/semaphore_impl.hpp"
#include "nickel/graphics/internal/shader_module_impl.hpp"
#include "nickel/graphics/sampler.hpp"
#include "nickel/graphics/semaphore.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

class DeviceImpl {
public:
    struct QueueFamilyIndices {
        std::optional<uint32_t> m_graphics_index;
        std::optional<uint32_t> m_present_index;

        explicit operator bool() const {
            return m_graphics_index && m_present_index;
        }

        std::set<uint32_t> GetUniqueIndices() const {
            return {m_graphics_index.value(), m_present_index.value()};
        }

        bool HasSeparateQueue() const {
            return m_graphics_index.value() != m_present_index.value();
        }

        std::vector<uint32_t> GetIndices() const {
            std::vector<uint32_t> indices;
            if (!m_graphics_index.has_value() || !m_present_index.has_value()) {
                return {};
            }

            if (m_graphics_index.has_value() == m_present_index.has_value()) {
                indices.push_back(m_graphics_index.value());
            } else {
                indices.push_back(m_graphics_index.value());
                indices.push_back(m_present_index.value());
            }
            return indices;
        }
    };

    struct AcquiredSwapchainImage {
        std::vector<ImageView>  m_views;
        uint32_t m_using_index{};
    };

    DeviceImpl(const AdapterImpl&, const SVector<uint32_t, 2>& window_size);
    DeviceImpl(const DeviceImpl&) = delete;
    DeviceImpl(DeviceImpl&&) = delete;
    DeviceImpl& operator=(const DeviceImpl&) = delete;
    DeviceImpl& operator=(DeviceImpl&&) = delete;

    ~DeviceImpl();

    const SwapchainImageInfo& GetSwapchainImageInfo() const noexcept;

    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_present_queue = VK_NULL_HANDLE;
    VkQueue m_graphics_queue = VK_NULL_HANDLE;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<ImageViewImpl*> m_swapchain_image_views;
    QueueFamilyIndices m_queue_indices;

    Buffer CreateBuffer(const Buffer::Descriptor&);
    Image CreateImage(const Image::Descriptor&);
    ImageView CreateImageView(const Image& image, const ImageView::Descriptor&);
    BindGroupLayout CreateBindGroupLayout(const BindGroupLayout::Descriptor&);
    PipelineLayout CreatePipelineLayout(const PipelineLayout::Descriptor&);
    Framebuffer CreateFramebuffer(const Framebuffer::Descriptor&);
    RenderPass CreateRenderPass(const RenderPass::Descriptor&);
    GraphicsPipeline CreateGraphicPipeline(const GraphicsPipeline::Descriptor&);
    Sampler CreateSampler(const Sampler::Descriptor&);
    ShaderModule CreateShaderModule(const uint32_t* data, size_t size);
    Semaphore CreateSemaphore();
    Fence CreateFence(bool signaled);
    CommandEncoder CreateCommandEncoder();
    uint32_t WaitAndAcquireSwapchainImageIndex();
    std::vector<ImageView> GetSwapchainImageViews() const;

    const AdapterImpl& GetAdapter() const;
    
    void Submit(Command&);
    void WaitIdle();

    void EndFrame();

    std::vector<BufferImpl*> m_pending_delete_buffers;
    std::vector<ImageImpl*> m_pending_delete_images;
    std::vector<ImageViewImpl*> m_pending_delete_image_views;
    std::vector<BindGroupLayoutImpl*> m_pending_delete_bind_group_layouts;
    std::vector<FramebufferImpl*> m_pending_delete_framebuffers;
    std::vector<GraphicsPipelineImpl*> m_pending_delete_graphics_pipelines;
    std::vector<RenderPassImpl*> m_pending_delete_render_passes;
    std::vector<SamplerImpl*> m_pending_delete_samplers;
    std::vector<ShaderModuleImpl*> m_pending_delete_shader_modules;
    std::vector<PipelineLayoutImpl*> m_pending_delete_pipeline_layouts;
    std::vector<SemaphoreImpl*> m_pending_delete_semaphores;
    std::vector<FenceImpl*> m_pending_delete_fences;

private:
    SwapchainImageInfo m_image_info;
    const AdapterImpl& m_adapter;
    uint32_t m_cur_swapchain_image_index = 0;
    uint32_t m_cur_frame = 0;
    std::vector<Fence> m_render_fences;
    std::vector<Semaphore> m_image_avaliable_sems;
    std::vector<Semaphore> m_render_finish_sems;
    std::vector<CommandPoolImpl*> m_cmd_pools;
    std::vector<bool> m_need_present;

    BlockMemoryAllocator<BufferImpl> m_buffer_allocator;
    BlockMemoryAllocator<ImageImpl> m_image_allocator;
    BlockMemoryAllocator<ImageViewImpl> m_image_view_allocator;
    BlockMemoryAllocator<BindGroupLayoutImpl> m_bind_group_layout_allocator;
    BlockMemoryAllocator<FramebufferImpl> m_framebuffer_allocator;
    BlockMemoryAllocator<GraphicsPipelineImpl> m_graphics_pipeline_allocator;
    BlockMemoryAllocator<RenderPassImpl> m_render_pass_allocator;
    BlockMemoryAllocator<SamplerImpl> m_sampler_allocator;
    BlockMemoryAllocator<ShaderModuleImpl> m_shader_module_allocator;
    BlockMemoryAllocator<PipelineLayoutImpl> m_pipeline_layout_allocator;
    BlockMemoryAllocator<SemaphoreImpl> m_semaphore_allocator;
    BlockMemoryAllocator<FenceImpl> m_fence_allocator;

    QueueFamilyIndices chooseQueue(VkPhysicalDevice phyDevice,
                                   VkSurfaceKHR surface);

    void createSwapchain(VkPhysicalDevice phyDev, VkSurfaceKHR surface);
    SwapchainImageInfo queryImageInfo(VkPhysicalDevice,
                                      const SVector<uint32_t, 2>&,
                                      VkSurfaceKHR);
    VkPresentModeKHR queryPresentMode(VkPhysicalDevice, VkSurfaceKHR);
    void createCmdPools();

    void getAndCreateImageViews();
    void createRenderRelateSyncObjs();
    void cleanUpOneFrame();
};


}  // namespace nickel::graphics