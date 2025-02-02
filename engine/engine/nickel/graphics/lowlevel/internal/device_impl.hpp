#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/graphics/lowlevel/cmd.hpp"
#include "nickel/graphics/lowlevel/cmd_encoder.hpp"
#include "nickel/graphics/lowlevel/device.hpp"
#include "nickel/graphics/lowlevel/fence.hpp"
#include "nickel/graphics/lowlevel/internal/bind_group_layout_impl.hpp"
#include "nickel/graphics/lowlevel/internal/buffer_impl.hpp"
#include "nickel/graphics/lowlevel/internal/cmd_pool_impl.hpp"
#include "nickel/graphics/lowlevel/internal/fence_impl.hpp"
#include "nickel/graphics/lowlevel/internal/framebuffer_impl.hpp"
#include "nickel/graphics/lowlevel/internal/graphics_pipeline_impl.hpp"
#include "nickel/graphics/lowlevel/internal/image_impl.hpp"
#include "nickel/graphics/lowlevel/internal/image_view_impl.hpp"
#include "nickel/graphics/lowlevel/internal/pipeline_layout_impl.hpp"
#include "nickel/graphics/lowlevel/internal/render_pass_impl.hpp"
#include "nickel/graphics/lowlevel/internal/sampler_impl.hpp"
#include "nickel/graphics/lowlevel/internal/semaphore_impl.hpp"
#include "nickel/graphics/lowlevel/internal/shader_module_impl.hpp"
#include "nickel/graphics/lowlevel/sampler.hpp"
#include "nickel/graphics/lowlevel/semaphore.hpp"
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
    std::vector<ImageView> m_swapchain_image_views;
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
    uint32_t WaitAndAcquireSwapchainImageIndex(Semaphore signal_sem, std::span<Fence>);
    std::vector<ImageView> GetSwapchainImageViews() const;

    const AdapterImpl& GetAdapter() const;

    void Submit(Command&, std::span<Semaphore> wait_sems,
                std::span<Semaphore> signal_sems, Fence fence);
    void WaitIdle();

    void EndFrame();

    void Present(std::span<Semaphore> semaphores);
    
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

private:
    SwapchainImageInfo m_image_info;
    const AdapterImpl& m_adapter;
    uint32_t m_cur_swapchain_image_index = 0;
    uint32_t m_cur_frame = 0;
    std::vector<CommandPoolImpl*> m_cmd_pools;

    QueueFamilyIndices chooseQueue(VkPhysicalDevice phyDevice,
                                   VkSurfaceKHR surface);

    void createSwapchain(VkPhysicalDevice phyDev, VkSurfaceKHR surface);
    SwapchainImageInfo queryImageInfo(VkPhysicalDevice,
                                      const SVector<uint32_t, 2>&,
                                      VkSurfaceKHR);
    VkPresentModeKHR queryPresentMode(VkPhysicalDevice, VkSurfaceKHR);
    void createCmdPools();

    void getAndCreateImageViews();
    void cleanUpOneFrame();
};


}  // namespace nickel::graphics