#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/common/log.hpp"
#include "nickel/graphics/internal/adapter_impl.hpp"
#include "nickel/graphics/internal/cmd_impl.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

DeviceImpl::DeviceImpl(const AdapterImpl& impl,
                       const SVector<uint32_t, 2>& window_size)
    : m_adapter{impl} {
    m_queue_indices = chooseQueue(impl.m_phyDevice, impl.m_surface);

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

    std::vector<const char*> requireExtensions;
#ifdef NICKEL_DEBUG
    requireExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#endif
    std::vector<VkExtensionProperties> extension_props;
    uint32_t extensionCount = 0;
    VK_CALL(vkEnumerateDeviceExtensionProperties(impl.m_phyDevice, nullptr,
                                                 &extensionCount, nullptr));
    extension_props.resize(extensionCount);
    VK_CALL(vkEnumerateDeviceExtensionProperties(
        impl.m_phyDevice, nullptr, &extensionCount, extension_props.data()));

    RemoveUnexistsElems<const char*, VkExtensionProperties>(
        requireExtensions, extension_props,
        [](const auto& e1, const VkExtensionProperties& e2) {
            return std::strcmp(e1, e2.extensionName) == 0;
        });

    for (auto ext : requireExtensions) {
        LOGI("enable vulkan device extension: {}", ext);
    }

    std::vector<const char*> extension_names;
    for (auto ext : requireExtensions) {
        extension_names.push_back(ext);
    }

    device_ci.ppEnabledExtensionNames = extension_names.data();
    device_ci.enabledExtensionCount = extension_names.size();

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(impl.m_phyDevice, &features);

    // features.geometryShader = true;
    device_ci.pEnabledFeatures = &features;

    VK_CALL(vkCreateDevice(impl.m_phyDevice, &device_ci, nullptr, &m_device));

    if (!m_device) {
        LOGC("failed to create vulkan device");
    }
    volkLoadDevice(m_device);

    vkGetDeviceQueue(m_device, m_queue_indices.m_graphics_index.value(), 0,
                     &m_graphics_queue);
    vkGetDeviceQueue(m_device, m_queue_indices.m_present_index.value(), 0,
                     &m_present_queue);

    createSwapchain(impl.m_phyDevice, impl.m_surface, window_size);
    createRenderRelateSyncObjs();
    createDefaultCmdPool();
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

void DeviceImpl::createSwapchain(VkPhysicalDevice phyDev, VkSurfaceKHR surface,
                                 const SVector<uint32_t, 2>& window_size) {
    VkSurfaceCapabilitiesKHR capacities;
    VK_CALL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phyDev, surface,
                                                      &capacities));

    m_image_info = queryImageInfo(phyDev, window_size, surface);
    auto [extent, imageCount, format] = m_image_info;
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
    createInfo.minImageCount = imageCount;
    createInfo.imageUsage =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    createInfo.imageExtent.width = extent.w;
    createInfo.imageExtent.height = extent.h;
    createInfo.imageFormat = format.format;
    createInfo.imageColorSpace = format.colorSpace;
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
        getAndCreateImageViews();
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
        {extent.width, extent.height},
        imageCount, *chooseFormat
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

void DeviceImpl::getAndCreateImageViews() {
    std::vector<VkImage> images;
    uint32_t count = 0;
    VK_CALL(vkGetSwapchainImagesKHR(m_device, m_swapchain, &count, nullptr));
    images.resize(count);
    VK_CALL(
        vkGetSwapchainImagesKHR(m_device, m_swapchain, &count, images.data()));

    for (auto& image : images) {
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
        ci.format = m_image_info.format.format;
        ci.subresourceRange = range;

        VkImageView view;
        VK_CALL(vkCreateImageView(m_device, &ci, nullptr, &view));
        if (view) {
            m_swapchain_image_views.push_back(
                new ImageViewImpl{m_device, view});
        } else {
            LOGC("create image view from swapchain image failed");
        }
    }
}

void DeviceImpl::createRenderRelateSyncObjs() {
    for (uint32_t i = 0; i < m_image_info.imagCount; i++) {
        m_render_fences.push_back(CreateFence(false));
        m_image_avaliable_sems.push_back(CreateSemaphore());
        m_render_finish_sems.push_back(CreateSemaphore());
    }
}

void DeviceImpl::createDefaultCmdPool() {
    m_cmdpool =
        CreateCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
}

DeviceImpl::~DeviceImpl() {
    for (auto view : m_swapchain_image_views) {
        delete view;
    }
    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    m_samplers.Clear();
    m_image_views.Clear();
    m_images.Clear();
    m_render_passes.Clear();
    m_buffers.Clear();
    m_pipeline_layout.Clear();
    m_bind_group_layouts.Clear();
    m_fbos.Clear();
    m_graphic_pipelines.Clear();
    m_pools.Clear();
    m_semaphores.Clear();
    m_fences.Clear();
    vkDestroyDevice(m_device, nullptr);
}

const SwapchainImageInfo& DeviceImpl::GetSwapchainImageInfo()
    const noexcept {
    return m_image_info;
}

Buffer DeviceImpl::CreateBuffer(const Buffer::Descriptor& desc) {
    BufferImpl* impl = m_buffers.Allocate(*this, m_adapter.m_phyDevice, desc);
    return Buffer{impl};
}

Image DeviceImpl::CreateImage(const Image::Descriptor& desc) {
    return Image{m_images.Allocate(m_adapter, *this, desc)};
}

ImageView DeviceImpl::CreateImageView(const Image& image,
                                      const ImageView::Descriptor& desc) {
    return ImageView{m_image_views.Allocate(*this, image, desc)};
}

BindGroupLayout DeviceImpl::CreateBindGroupLayout(
    const BindGroupLayout::Descriptor& desc) {
    return BindGroupLayout{m_bind_group_layouts.Allocate(*this, desc)};
}

PipelineLayout DeviceImpl::CreatePipelineLayout(
    const PipelineLayout::Descriptor& desc) {
    return PipelineLayout{m_pipeline_layout.Allocate(*this, desc)};
}

Framebuffer DeviceImpl::CreateFramebuffer(const Framebuffer::Descriptor& desc) {
    return Framebuffer{m_fbos.Allocate(*this, desc)};
}

RenderPass DeviceImpl::CreateRenderPass(const RenderPass::Descriptor& desc) {
    return RenderPass{m_render_passes.Allocate(*this, desc)};
}

GraphicsPipeline DeviceImpl::CreateGraphicPipeline(
    const GraphicsPipeline::Descriptor& desc) {
    return GraphicsPipeline{m_graphic_pipelines.Allocate(*this, desc)};
}

Sampler DeviceImpl::CreateSampler(const Sampler::Descriptor& desc) {
    return Sampler{m_samplers.Allocate(*this, desc)};
}

ShaderModule DeviceImpl::CreateShaderModule(const uint32_t* data, size_t size) {
    return ShaderModule{m_shader_modules.Allocate(m_device, data, size)};
}

CommandPool DeviceImpl::CreateCommandPool(VkCommandPoolCreateFlags flags) {
    return CommandPool{m_pools.Allocate(*this, flags)};
}

Semaphore DeviceImpl::CreateSemaphore() {
    return Semaphore{m_semaphores.Allocate(*this)};
}

Fence DeviceImpl::CreateFence(bool signaled) {
    return Fence{m_fences.Allocate(*this, signaled)};
}

void DeviceImpl::Submit(Command& cmd) {
    std::vector<VkCommandBuffer> bufs;
    bufs.push_back(cmd.Impl().m_cmd);

    if (cmd.Impl().m_flags & CommandImpl::Flag::Render) {
        VkFence fence = m_render_fences[m_cur_frame].Impl().m_fence;

        VkSubmitInfo info;
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.commandBufferCount = 1;
        info.pCommandBuffers = bufs.data();

        VkPipelineStageFlags waitDstStage =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        info.pWaitDstStageMask = &waitDstStage;
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores =
            &m_render_finish_sems[m_cur_frame].Impl().m_semaphore;
        info.pWaitSemaphores =
            &m_image_avaliable_sems[m_cur_frame].Impl().m_semaphore;
        info.waitSemaphoreCount = 1;

        VK_CALL(vkQueueSubmit(m_graphics_queue, 1, &info, fence));
    } else {
        VkSubmitInfo info;
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.commandBufferCount = 1;
        info.pCommandBuffers = bufs.data();

        VK_CALL(vkQueueSubmit(m_graphics_queue, 1, &info, VK_NULL_HANDLE));
        WaitIdle();
    }
    cmd.Impl().ApplyLayoutTransitions();
}

void DeviceImpl::WaitIdle() {
    VK_CALL(vkDeviceWaitIdle(m_device));
}

void DeviceImpl::AcquireSwapchainImageAndWait(video::Window& window) {
    if (window.IsMinimize()) {
        return;
    }

    VK_CALL(vkAcquireNextImageKHR(
        m_device, m_swapchain, UINT64_MAX,
        m_image_avaliable_sems[m_cur_frame].Impl().m_semaphore, VK_NULL_HANDLE,
        &m_cur_swapchain_image_index));

    VkFence fence = m_render_fences[m_cur_frame].Impl().m_fence;
    VK_CALL(vkWaitForFences(m_device, 1, &fence, true, UINT64_MAX));
    VK_CALL(vkResetFences(m_device, 1, &fence));

    m_cmdpool.Reset();

    VkPresentInfoKHR info;
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.pImageIndices = &m_cur_swapchain_image_index;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores =
        &m_render_finish_sems[m_cur_frame].Impl().m_semaphore;
    info.swapchainCount = 1;
    info.pSwapchains = &m_swapchain;

    VK_CALL(vkQueuePresentKHR(m_present_queue, &info));

    m_cur_frame = (m_cur_frame + 1) % m_image_info.imagCount;
}

}  // namespace nickel::graphics
