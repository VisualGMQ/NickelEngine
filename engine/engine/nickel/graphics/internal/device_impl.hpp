#pragma once
#include "nickel/graphics/cmd.hpp"
#include "nickel/graphics/cmd_pool.hpp"
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
#include "nickel/graphics/internal/shader_module_impl.hpp"
#include "nickel/graphics/sampler.hpp"
#include "nickel/graphics/semaphore.hpp"
#include "nickel/internal/pch.hpp"
#include "nickel/video/window.hpp"
#include "sampler_impl.hpp"
#include "semaphore_impl.hpp"

namespace nickel::graphics {

template <typename T>
class ResourceManager {
public:
    template <typename... Args>
    T* Allocate(Args&&... args) {
        if (m_unused_index >= m_resources.size()) {
            m_resources.push_back(new T(std::forward<Args>(args)...));
            m_unused_index++;
            return m_resources.back();
        }

        new (m_resources[m_unused_index]) T{std::forward<Args>(args)...};
        return m_resources[m_unused_index++];
    }

    void Free(T* resource) {
        auto it = std::find(m_resources.begin(),
                            m_resources.begin() + m_unused_index, resource);
        if (it != m_resources.end()) {
            resource->~T();
            std::swap(m_resources[--m_unused_index], *it);
        } else {
            LOGE("try to free an unmanaged resource");
        }
    }

    void Clear() {
        size_t i = 0;
        for (; i < m_unused_index; i++) {
            delete m_resources[i];
        }

        for (; i < m_resources.size(); i++) {
            free(m_resources[i]);
        }

        m_unused_index = 0;
        m_resources.clear();
        m_resources.shrink_to_fit();
    }

private:
    std::vector<T*> m_resources;
    size_t m_unused_index{};
};

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

    struct ImageInfo {
        SVector<uint32_t, 2> extent;
        uint32_t imagCount;
        VkSurfaceFormatKHR format;
    };

    DeviceImpl(const AdapterImpl&, const SVector<uint32_t, 2>& window_size);
    ~DeviceImpl();

    const ImageInfo& GetSwapchainImageInfo() const noexcept;

    VkDevice m_device;
    VkQueue m_present_queue;
    VkQueue m_graphics_queue;
    VkSwapchainKHR m_swapchain;
    std::vector<ImageViewImpl*> m_swapchain_image_views;
    QueueFamilyIndices m_queue_indices;

    Buffer CreateBuffer(const Buffer::Descriptor&);
    Image CreateImage(const Image::Descriptor&);
    BindGroupLayout CreateBindGroupLayout(const BindGroupLayout::Descriptor&);
    PipelineLayout CreatePipelineLayout(const PipelineLayout::Descriptor&);
    Framebuffer CreateFramebuffer(const Framebuffer::Descriptor&);
    RenderPass CreateRenderPass(const RenderPass::Descriptor&);
    GraphicsPipeline CreateGraphicPipeline(const GraphicsPipeline::Descriptor&);
    Sampler CreateSampler(const Sampler::Descriptor&);
    ShaderModule CreateShaderModule(const uint32_t* data, size_t size);
    CommandPool CreateCommandPool(VkCommandPoolCreateFlags flags);
    Semaphore CreateSemaphore();
    Fence CreateFence(bool signaled);
    void Submit(const Command&);
    void WaitIdle();
    
    void AcquireSwapchainImageAndWait(video::Window& window);

    ResourceManager<BufferImpl> m_buffers;
    ResourceManager<ImageImpl> m_images;
    ResourceManager<BindGroupLayoutImpl> m_bind_group_layouts;
    ResourceManager<CommandPoolImpl> m_pools;
    ResourceManager<FramebufferImpl> m_fbos;
    ResourceManager<GraphicsPipelineImpl> m_graphic_pipelines;
    ResourceManager<RenderPassImpl> m_render_passes;
    ResourceManager<SamplerImpl> m_samplers;
    ResourceManager<ShaderModuleImpl> m_shader_modules;
    ResourceManager<PipelineLayoutImpl> m_pipeline_layout;
    ResourceManager<SemaphoreImpl> m_semaphores;
    ResourceManager<FenceImpl> m_fences;

private:
    ImageInfo m_image_info;
    const AdapterImpl& m_adapter;
    uint32_t m_cur_swapchain_image_index = 0;
    uint32_t m_cur_frame = 0;
    std::vector<Fence> m_render_fences;
    std::vector<Semaphore> m_image_avaliable_sems;
    std::vector<Semaphore> m_render_finish_sems;
    CommandPool m_cmdpool;

    QueueFamilyIndices chooseQueue(VkPhysicalDevice phyDevice,
                                   VkSurfaceKHR surface);

    void createSwapchain(VkPhysicalDevice phyDev, VkSurfaceKHR surface,
                         const SVector<uint32_t, 2>& window_size);
    ImageInfo queryImageInfo(VkPhysicalDevice, const SVector<uint32_t, 2>&,
                             VkSurfaceKHR);
    VkPresentModeKHR queryPresentMode(VkPhysicalDevice, VkSurfaceKHR);

    void getAndCreateImageViews();
    void createRenderRelateSyncObjs();
    void createDefaultCmdPool();
};

}  // namespace nickel::graphics