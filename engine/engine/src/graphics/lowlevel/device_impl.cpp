#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/common/log.hpp"
#include "nickel/common/macro.hpp"
#include "nickel/graphics/lowlevel/fence.hpp"
#include "nickel/graphics/lowlevel/internal/adapter_impl.hpp"
#include "nickel/graphics/lowlevel/internal/cmd_impl.hpp"
#include "nickel/graphics/lowlevel/internal/enum_convert.hpp"
#include "nickel/graphics/lowlevel/internal/fence_impl.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"
#include "nickel/graphics/lowlevel/internal/semaphore_impl.hpp"

namespace nickel::graphics {

DeviceImpl::DeviceImpl(const AdapterImpl& impl,
                       const SVector<uint32_t, 2>& window_size)
    : m_adapter{impl} {
    m_queue_indices = chooseQueue(impl.m_phy_device, impl.m_surface);

    if (!m_queue_indices) {
        LOGC("no graphics queue in your GPU");
    }

    VkDeviceCreateInfo device_ci{};
    device_ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::set indices{m_queue_indices.m_graphics_index.value(),
                     m_queue_indices.m_present_index.value()};

    float priority = 1.0;
    for (auto idx : indices) {
        VkDeviceQueueCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        ci.queueCount = 1;
        ci.queueFamilyIndex = idx;
        ci.pQueuePriorities = &priority;
        queueCreateInfos.push_back(ci);
    }
    device_ci.queueCreateInfoCount = queueCreateInfos.size();
    device_ci.pQueueCreateInfos = queueCreateInfos.data();

    std::vector<const char*> requireExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME};
    std::vector<VkExtensionProperties> extension_props;
    uint32_t extensionCount = 0;
    VK_CALL(vkEnumerateDeviceExtensionProperties(impl.m_phy_device, nullptr,
                                                 &extensionCount, nullptr));

    extension_props.resize(extensionCount);
    VK_CALL(vkEnumerateDeviceExtensionProperties(
        impl.m_phy_device, nullptr, &extensionCount, extension_props.data()));

    size_t required_extensions_size = requireExtensions.size();

    RemoveUnexistsElems<const char*, VkExtensionProperties>(
        requireExtensions, extension_props,
        [](const auto& e1, const VkExtensionProperties& e2) {
            return std::strcmp(e1, e2.extensionName) == 0;
        });

    for (auto ext : requireExtensions) {
        LOGI("enable vulkan device extension: {}", ext);
    }

    if (requireExtensions.size() != required_extensions_size) {
        LOGC("some vulkan extension not support");
    }

    std::vector<const char*> extension_names;
    for (auto ext : requireExtensions) {
        extension_names.push_back(ext);
    }

    device_ci.ppEnabledExtensionNames = extension_names.data();
    device_ci.enabledExtensionCount = extension_names.size();

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(impl.m_phy_device, &features);

    // features.geometryShader = true;
    device_ci.pEnabledFeatures = &features;

    VK_CALL(vkCreateDevice(impl.m_phy_device, &device_ci, nullptr, &m_device));

    if (!m_device) {
        LOGC("failed to create vulkan device");
    }
    volkLoadDevice(m_device);

    vkGetDeviceQueue(m_device, m_queue_indices.m_graphics_index.value(), 0,
                     &m_graphics_queue);
    vkGetDeviceQueue(m_device, m_queue_indices.m_present_index.value(), 0,
                     &m_present_queue);

    m_image_info =
        queryImageInfo(impl.m_phy_device, window_size, impl.m_surface);
    createCmdPools();
    createSwapchain(impl.m_phy_device, impl.m_surface);
}

DeviceImpl::QueueFamilyIndices DeviceImpl::chooseQueue(
    VkPhysicalDevice phyDevice, VkSurfaceKHR surface) {
    uint32_t count = 0;
    std::vector<VkQueueFamilyProperties> queue_families;
    vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &count, nullptr);
    queue_families.resize(count);
    vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &count,
                                             queue_families.data());

    QueueFamilyIndices indices;

    for (int i = 0; i < queue_families.size(); i++) {
        auto& prop = queue_families[i];
        if (prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.m_graphics_index = i;
        }
        VkBool32 supportSurface = false;
        VK_CALL(vkGetPhysicalDeviceSurfaceSupportKHR(phyDevice, i, surface,
                                                     &supportSurface));
        if (supportSurface) {
            indices.m_present_index = i;
        }

        if (indices) {
            break;
        }
    }

    return indices;
}

void DeviceImpl::createSwapchain(VkPhysicalDevice phyDev,
                                 VkSurfaceKHR surface) {
    VkSurfaceCapabilitiesKHR capacities;
    VK_CALL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phyDev, surface,
                                                      &capacities));
    auto presentMode = queryPresentMode(phyDev, surface);

    auto& queueIndices = m_queue_indices;

    std::set uniqueIndices{queueIndices.m_graphics_index.value(),
                           queueIndices.m_present_index.value()};

    std::vector<uint32_t> indices;
    for (auto idx : uniqueIndices) {
        indices.push_back(idx);
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.clipped = true;
    createInfo.presentMode = presentMode;
    createInfo.minImageCount = m_image_info.m_image_count;
    createInfo.imageUsage =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    createInfo.imageExtent.width = m_image_info.m_extent.w;
    createInfo.imageExtent.height = m_image_info.m_extent.h;
    createInfo.imageFormat = Format2Vk(m_image_info.m_surface_format.format);
    createInfo.imageColorSpace =
        ImageColorSpace2Vk(m_image_info.m_surface_format.colorSpace);
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.imageArrayLayers = 1;
    createInfo.preTransform = capacities.currentTransform;
    createInfo.imageSharingMode = queueIndices.m_graphics_index.value() ==
                                          queueIndices.m_present_index.value()
                                      ? VK_SHARING_MODE_EXCLUSIVE
                                      : VK_SHARING_MODE_CONCURRENT;
    createInfo.pQueueFamilyIndices = indices.data();
    createInfo.queueFamilyIndexCount = indices.size();
    VK_CALL(vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain));

    if (m_swapchain) {
        getAndCreateSwapchainImageViews();
    } else {
        LOGC("swapchain create failed");
    }
}

SwapchainImageInfo DeviceImpl::queryImageInfo(
    VkPhysicalDevice dev, const SVector<uint32_t, 2>& win_size,
    VkSurfaceKHR surface) {
    VkSurfaceCapabilitiesKHR capacities;
    VK_CALL(
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev, surface, &capacities));

    VkExtent2D extent{win_size.x, win_size.y};
    extent.width = Clamp(extent.width, capacities.minImageExtent.width,
                         capacities.maxImageExtent.width);
    extent.height = Clamp(extent.height, capacities.minImageExtent.height,
                          capacities.maxImageExtent.height);

    auto imageCount =
        Clamp<uint32_t>(2, capacities.minImageCount, capacities.maxImageCount);

    std::vector<VkSurfaceFormatKHR> formats;
    uint32_t count = 0;
    VK_CALL(
        vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &count, nullptr));
    formats.resize(count);
    VK_CALL(vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &count,
                                                 formats.data()));
    VkSurfaceFormatKHR* chooseFormat = &formats[0];
    for (auto& format : formats) {
        if (format.format == VK_FORMAT_R8G8B8A8_UNORM &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            chooseFormat = &format;
            break;
        }
    }

    return {
        {                extent.width,extent.height                                      },
        imageCount,
        SurfaceFormatKHR{
         VkFormat2Format(chooseFormat->format),
         VkColorSpace2ImageColorSpace(chooseFormat->colorSpace)}
    };
}

VkPresentModeKHR DeviceImpl::queryPresentMode(VkPhysicalDevice dev,
                                              VkSurfaceKHR surface) {
    std::vector<VkPresentModeKHR> presentModes;
    uint32_t count = 0;
    VK_CALL(vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &count,
                                                      nullptr));
    presentModes.resize(count);
    VK_CALL(vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &count,
                                                      presentModes.data()));
    VkPresentModeKHR choose = VK_PRESENT_MODE_FIFO_KHR;
    for (auto mode : presentModes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            choose = mode;
            break;
        }
    }

    return choose;
}

void DeviceImpl::createCmdPools() {
    for (int i = 0; i < m_image_info.m_image_count; i++) {
        m_cmd_pools.push_back(new CommandPoolImpl(*this, 0));
    }
}

void DeviceImpl::getAndCreateSwapchainImageViews() {
    std::vector<VkImage> images;
    uint32_t count = 0;
    VK_CALL(vkGetSwapchainImagesKHR(m_device, m_swapchain, &count, nullptr));
    images.resize(count);
    VK_CALL(
        vkGetSwapchainImagesKHR(m_device, m_swapchain, &count, images.data()));

    for (auto image : images) {
        VkImageSubresourceRange range;
        range.levelCount = 1;
        range.layerCount = 1;
        range.baseArrayLayer = 0;
        range.baseMipLevel = 0;
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        VkImageViewCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ci.image = image;
        ci.components.r = VK_COMPONENT_SWIZZLE_R;
        ci.components.g = VK_COMPONENT_SWIZZLE_G;
        ci.components.b = VK_COMPONENT_SWIZZLE_B;
        ci.components.a = VK_COMPONENT_SWIZZLE_A;
        ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ci.format = Format2Vk(m_image_info.m_surface_format.format);
        ci.subresourceRange = range;

        VkImageView view;
        VK_CALL(vkCreateImageView(m_device, &ci, nullptr, &view));
        if (view) {
            m_swapchain_image_views.push_back(
                ImageView{m_image_view_allocator.Allocate(*this, view)});
        } else {
            LOGC("create image m_view from swapchain image failed");
        }
    }

    // layout transition
    VkCommandBufferAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandBufferCount = 1;
    info.commandPool = m_cmd_pools[0]->m_pool;
    VkCommandBuffer cmd;
    VK_CALL(vkAllocateCommandBuffers(m_device, &info, &cmd));

    std::vector<VkImageMemoryBarrier> barriers;
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VK_CALL(vkBeginCommandBuffer(cmd, &begin_info));
    for (auto image : images) {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcAccessMask = VK_ACCESS_NONE;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barriers.push_back(barrier);
    }
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0, nullptr, 0,
                         nullptr, barriers.size(), barriers.data());
    VK_CALL(vkEndCommandBuffer(cmd));

    VkSubmitInfo submit{};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &cmd;
    VK_CALL(vkQueueSubmit(m_graphics_queue, 1, &submit, VK_NULL_HANDLE));
    WaitIdle();

    VK_CALL(vkResetCommandPool(m_device, m_cmd_pools[0]->m_pool, 0));
}

DeviceImpl::~DeviceImpl() {
    WaitIdle();

    m_swapchain_image_views.clear();
    m_bind_group_layout_allocator.FreeAll();

    m_framebuffer_allocator.FreeAll();
    m_bind_group_layout_allocator.FreeAll();
    m_image_view_allocator.FreeAll();
    m_image_allocator.FreeAll();

    m_shader_module_allocator.FreeAll();

    m_buffer_allocator.FreeAll();
    m_sampler_allocator.FreeAll();
    m_graphics_pipeline_allocator.FreeAll();
    m_pipeline_layout_allocator.FreeAll();
    m_render_pass_allocator.FreeAll();

    m_semaphore_allocator.FreeAll();
    m_fence_allocator.FreeAll();
    for (auto pool : m_cmd_pools) {
        delete pool;
    }
    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    vkDestroyDevice(m_device, nullptr);
}

void DeviceImpl::cleanUpOneFrame() {
    m_shader_module_allocator.GC();
    m_bind_group_layout_allocator.GC();
    m_framebuffer_allocator.GC();
    m_image_view_allocator.GC();
    m_image_allocator.GC();
    m_render_pass_allocator.GC();
    m_sampler_allocator.GC();
    m_buffer_allocator.GC();
    m_pipeline_layout_allocator.GC();
    m_graphics_pipeline_allocator.GC();
    m_semaphore_allocator.GC();
    m_fence_allocator.GC();
}

const SwapchainImageInfo& DeviceImpl::GetSwapchainImageInfo() const noexcept {
    return m_image_info;
}

Buffer DeviceImpl::CreateBuffer(const Buffer::Descriptor& desc) {
    return Buffer{
        m_buffer_allocator.Allocate(*this, m_adapter.m_phy_device, desc)};
}

Image DeviceImpl::CreateImage(const Image::Descriptor& desc) {
    return Image{m_image_allocator.Allocate(m_adapter, *this, desc)};
}

ImageView DeviceImpl::CreateImageView(const Image& image,
                                      const ImageView::Descriptor& desc) {
    return ImageView{m_image_view_allocator.Allocate(*this, image, desc)};
}

BindGroupLayout DeviceImpl::CreateBindGroupLayout(
    const BindGroupLayout::Descriptor& desc) {
    return BindGroupLayout{m_bind_group_layout_allocator.Allocate(*this, desc)};
}

PipelineLayout DeviceImpl::CreatePipelineLayout(
    const PipelineLayout::Descriptor& desc) {
    return PipelineLayout{m_pipeline_layout_allocator.Allocate(*this, desc)};
}

Framebuffer DeviceImpl::CreateFramebuffer(const Framebuffer::Descriptor& desc) {
    return Framebuffer{m_framebuffer_allocator.Allocate(*this, desc)};
}

RenderPass DeviceImpl::CreateRenderPass(const RenderPass::Descriptor& desc) {
    return RenderPass{m_render_pass_allocator.Allocate(*this, desc)};
}

GraphicsPipeline DeviceImpl::CreateGraphicPipeline(
    const GraphicsPipeline::Descriptor& desc) {
    return GraphicsPipeline{
        m_graphics_pipeline_allocator.Allocate(*this, desc)};
}

Sampler DeviceImpl::CreateSampler(const Sampler::Descriptor& desc) {
    return Sampler{m_sampler_allocator.Allocate(*this, desc)};
}

ShaderModule DeviceImpl::CreateShaderModule(const uint32_t* data, size_t size) {
    return ShaderModule{m_shader_module_allocator.Allocate(*this, data, size)};
}

Semaphore DeviceImpl::CreateSemaphore() {
    return Semaphore{m_semaphore_allocator.Allocate(*this)};
}

Fence DeviceImpl::CreateFence(bool signaled) {
    return Fence{m_fence_allocator.Allocate(*this, signaled)};
}

CommandEncoder DeviceImpl::CreateCommandEncoder() {
    auto& cmd_pool = m_cmd_pools[m_cur_frame];
    return cmd_pool->CreateCommandEncoder();
}

void DeviceImpl::Submit(Command& cmd, std::span<Semaphore> wait_sems,
                        std::span<Semaphore> signal_sems, Fence fence) {
    VkSubmitInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.commandBufferCount = 1;
    info.pCommandBuffers = &cmd.Impl().m_cmd;

    VkPipelineStageFlags waitDstStage =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    std::vector<VkSemaphore> signal, wait;
    for (auto sem : wait_sems) {
        NICKEL_CONTINUE_IF_FALSE(sem);
        wait.push_back(sem.GetImpl()->m_semaphore);
    }
    for (auto sem : signal_sems) {
        NICKEL_CONTINUE_IF_FALSE(sem);
        signal.push_back(sem.GetImpl()->m_semaphore);
    }

    info.pWaitDstStageMask = &waitDstStage;
    info.signalSemaphoreCount = signal.size();
    info.pSignalSemaphores = signal.data();
    info.pWaitSemaphores = wait.data();
    info.waitSemaphoreCount = wait.size();

    VK_CALL(vkQueueSubmit(m_graphics_queue, 1, &info,
                          fence ? fence.GetImpl()->m_fence : VK_NULL_HANDLE));

    cmd.Impl().ApplyLayoutTransitions();
}

void DeviceImpl::WaitIdle() {
    VK_CALL(vkDeviceWaitIdle(m_device));
}

uint32_t DeviceImpl::WaitAndAcquireSwapchainImageIndex(
    Semaphore sem, std::span<Fence> fences) {
    std::vector<VkFence> vk_fences;
    vk_fences.reserve(fences.size());

    for (auto fence : fences) {
        NICKEL_CONTINUE_IF_FALSE(fence);
        vk_fences.push_back(fence.GetImpl()->m_fence);
    }

    VK_CALL(vkWaitForFences(m_device, vk_fences.size(), vk_fences.data(), true,
                            UINT64_MAX));
    VK_CALL(vkResetFences(m_device, vk_fences.size(), vk_fences.data()));
    m_cmd_pools[m_cur_frame]->Reset();

    vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX,
                          sem ? sem.GetImpl()->m_semaphore : VK_NULL_HANDLE,
                          VK_NULL_HANDLE, &m_cur_swapchain_image_index);

    return m_cur_swapchain_image_index;
}

std::vector<ImageView> DeviceImpl::GetSwapchainImageViews() const {
    return m_swapchain_image_views;
}

const AdapterImpl& DeviceImpl::GetAdapter() const {
    return m_adapter;
}

void DeviceImpl::EndFrame() {
    cleanUpOneFrame();
}

void DeviceImpl::Present(std::span<Semaphore> semaphores) {
    std::vector<VkSemaphore> vk_sems;
    vk_sems.reserve(semaphores.size());
    for (auto& semaphore : semaphores) {
        vk_sems.push_back(semaphore.GetImpl()->m_semaphore);
    }

    VkPresentInfoKHR info{};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.pImageIndices = &m_cur_swapchain_image_index;
    info.waitSemaphoreCount = vk_sems.size();
    info.pWaitSemaphores = vk_sems.data();
    info.swapchainCount = 1;
    info.pSwapchains = &m_swapchain;

    VK_CALL(vkQueuePresentKHR(m_present_queue, &info));
    m_cur_frame = (m_cur_frame + 1) % m_image_info.m_image_count;
}

void DeviceImpl::RecreateSwapchain(VkPhysicalDevice phy_device,
                                   const SVector<uint32_t, 2>& window_size,
                                   VkSurfaceKHR new_surface) {
    m_image_info = queryImageInfo(phy_device, window_size, new_surface);
    createSwapchain(phy_device, new_surface);
    getAndCreateSwapchainImageViews();
}

}  // namespace nickel::graphics
