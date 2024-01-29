#include "vulkan/device.hpp"
#include "core/log_tag.hpp"
#include "core/util.hpp"
#include "vulkan/util.hpp"

namespace nickel::vulkan {

Device::Device(Window& window) {
    createInstance(window);
    std::vector<vk::PhysicalDevice> phyDevices;
    VK_CALL(phyDevices, instance_.enumeratePhysicalDevices());
    choosePhysicalDevice(phyDevices);

    VkSurfaceKHR surface;
    SDL_Vulkan_CreateSurface((SDL_Window*)window.Raw(), instance_, &surface);
    
    if (!surface) {
        LOGW(nickel::log_tag::Vulkan, "create surface failed");
    }
    surface_ = surface;

    createDevice(instance_, surface);
    chooseQueue(phyDevice_, surface, phyDevice_.getQueueFamilyProperties());
    swapchain_ = std::make_unique<Swapchain>(*this, surface, window);
}

Device::~Device() {
    VK_CALL_NO_VALUE(device_.waitIdle());

    shaderModules_.ReleaseAll();
    buffers_.ReleaseAll();
    images_.ReleaseAll();
    framebuffers_.ReleaseAll();
    cmdPools_.ReleaseAll();
    semaphores_.ReleaseAll();
    fences_.ReleaseAll();
    events_.ReleaseAll();
    descriptorPools_.ReleaseAll();
    descriptorSetLayouts_.ReleaseAll();
    pipelineLayouts_.ReleaseAll();
    pipelines_.ReleaseAll();
    renderPasses_.ReleaseAll();

    swapchain_.reset();
    device_.destroy();
    instance_.destroySurfaceKHR(surface_);
    instance_.destroy();
}

void Device::createInstance(Window& window) {
    vk::InstanceCreateInfo createInfo;
    vk::ApplicationInfo appInfo;
    appInfo.setApiVersion(VK_API_VERSION_1_3);
    appInfo.setPEngineName("NickelEngine");
    createInfo.setPApplicationInfo(&appInfo);

    unsigned int count;
    SDL_Window* sdlWindow = (SDL_Window*)(window.Raw());
    SDL_Vulkan_GetInstanceExtensions(sdlWindow, &count, nullptr);
    std::vector<const char*> extensions(count);
    SDL_Vulkan_GetInstanceExtensions(sdlWindow, &count, extensions.data());

    for (auto& ext : extensions) {
        LOGW(nickel::log_tag::Vulkan, ext);
    }
    createInfo.setPEnabledExtensionNames(extensions);

    std::vector<vk::LayerProperties> supportLayers;
    VK_CALL(supportLayers, vk::enumerateInstanceLayerProperties());
    std::vector<const char*> requireLayers = {"VK_LAYER_KHRONOS_validation"};
    using LiteralString = const char*;
    RemoveUnexistsElems<const char*, vk::LayerProperties>(
        requireLayers, supportLayers,
        [](const LiteralString& require, const vk::LayerProperties& prop) {
            return std::strcmp(prop.layerName.data(), require) == 0;
        });

    createInfo.setPEnabledLayerNames(requireLayers);
    VK_CALL(instance_, vk::createInstance(createInfo));
}

void Device::createDevice(vk::Instance& instance, vk::SurfaceKHR surface) {
    std::vector<vk::PhysicalDevice> phyDevices;
    VK_CALL(phyDevices, instance.enumeratePhysicalDevices());

    choosePhysicalDevice(phyDevices);

    LOGW(nickel::log_tag::Vulkan, "choose GPU ",
         phyDevice_.getProperties().deviceName.data());

    queueIndices_ =
        chooseQueue(phyDevice_, surface, phyDevice_.getQueueFamilyProperties());

    if (!queueIndices_) {
        LOGW(nickel::log_tag::Vulkan, "no graphics queue in your GPU");
        return;
    }

    vk::DeviceCreateInfo createInfo;
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    std::set<uint32_t> indices{queueIndices_.graphicsIndex.value(),
                               queueIndices_.presentIndex.value()};

    float priority = 1.0;
    for (auto idx : indices) {
        vk::DeviceQueueCreateInfo createInfo;
        createInfo.setQueueCount(1);
        createInfo.setQueueFamilyIndex(idx);
        createInfo.setQueuePriorities(priority);
        queueCreateInfos.push_back(createInfo);
    }
    createInfo.setQueueCreateInfos(queueCreateInfos);

    std::vector<const char*> requireExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    std::vector<vk::ExtensionProperties> existsExtensions;
    VK_CALL(existsExtensions, phyDevice_.enumerateDeviceExtensionProperties());

    using LiteralString = const char*;
    RemoveUnexistsElems<const char*, vk::ExtensionProperties>(
        requireExtensions, existsExtensions,
        [](const LiteralString& e1, const vk::ExtensionProperties& e2) {
            return std::strcmp(e1, e2.extensionName) == 0;
        });

    for (auto ext : requireExtensions) {
        LOGW(nickel::log_tag::Vulkan, "enable extension on device: ", ext);
    }

    createInfo.setPEnabledExtensionNames(requireExtensions);

    VK_CALL(device_, phyDevice_.createDevice(createInfo));
    graphicsQueue_ = device_.getQueue(queueIndices_.graphicsIndex.value(), 0);
    presentQueue_ = device_.getQueue(queueIndices_.presentIndex.value(), 0);
}

int getDeviceTypeScore(vk::PhysicalDeviceType type) {
    switch (type) {
        case vk::PhysicalDeviceType::eDiscreteGpu:
            return 4;
        case vk::PhysicalDeviceType::eIntegratedGpu:
            return 3;
        case vk::PhysicalDeviceType::eVirtualGpu:
            return 2;
        case vk::PhysicalDeviceType::eCpu:
            return 1;
        default:
            return 0;
    }
}

void Device::choosePhysicalDevice(
    const std::vector<vk::PhysicalDevice>& phyDevices) {
    int chooseDevice = 0;
    int score = 0;
    for (int i = 0; i < phyDevices.size(); i++) {
        auto& phyDevice = phyDevices[i];
        auto prop = phyDevice.getProperties();
        auto feature = phyDevice.getFeatures();

        int curScore = getDeviceTypeScore(prop.deviceType);
        if (curScore > score) {
            score = curScore;
            chooseDevice = i;
        }
    }

    phyDevice_ = phyDevices[chooseDevice];
}

Device::QueueFamilyIndices Device::chooseQueue(
    vk::PhysicalDevice phyDevice, vk::SurfaceKHR surface,
    const std::vector<vk::QueueFamilyProperties>& props) {
    auto queueProps = phyDevice.getQueueFamilyProperties();

    QueueFamilyIndices indices;

    for (int i = 0; i < queueProps.size(); i++) {
        auto& prop = queueProps[i];
        if (prop.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphicsIndex = i;
        }
        bool supportSurface = false;
        VK_CALL(supportSurface, phyDevice.getSurfaceSupportKHR(i, surface));
        if (supportSurface) {
            indices.presentIndex = i;
        }

        if (indices) {
            break;
        }
    }

    return indices;
}

ResResult<ShaderModule> Device::CreateShaderModule(
    vk::ShaderStageFlagBits type, const std::filesystem::path& filename,
    const std::string& entry) {
    auto content = nickel::ReadWholeFile<std::vector<char>>(
        filename, std::ios_base::in | std::ios_base::binary);
    if (content) {
        return shaderModules_.Emplace(
            std::make_unique<ShaderModule>(this, type, content.value(), entry));
    }
    return {};
}

ResResult<Buffer> Device::CreateBuffer(uint64_t size,
                                       vk::BufferUsageFlags usage,
                                       vk::MemoryPropertyFlags flags,
                                       std::vector<uint32_t> queueIndices) {
    return buffers_.Emplace(
        std::make_unique<Buffer>(this, size, usage, flags, queueIndices));
}

ResResult<Image> Device::CreateImage(
    vk::ImageType type, vk::ImageViewType viewType, const vk::Extent3D& extent,
    vk::Format format, vk::Format viewFormat, vk::ImageLayout initLayout,
    uint32_t arrayLayer, uint32_t mipLevel, vk::SampleCountFlagBits sampleCount,
    vk::ImageUsageFlagBits usage, vk::ImageTiling tiling,
    const vk::ComponentMapping& components,
    const vk::ImageSubresourceRange& subresourceRange,
    std::vector<uint32_t> queueIndices) {
    return images_.Emplace(std::make_unique<Image>(
        this, type, viewType, extent, format, viewFormat, initLayout,
        arrayLayer, mipLevel, sampleCount, usage, tiling, components,
        subresourceRange, queueIndices));
}

ResResult<Pipeline> Device::CreateGraphicsPipeline(
    const VertexLayout& vertexLayout,
    const vk::PipelineInputAssemblyStateCreateInfo& inputAsm,
    const std::vector<ShaderModule*>& shaders,
    const vk::PipelineViewportStateCreateInfo& viewport,
    const vk::PipelineRasterizationStateCreateInfo& raster,
    const vk::PipelineDepthStencilStateCreateInfo& depthStencil,
    const vk::PipelineMultisampleStateCreateInfo& multisample,
    const vk::PipelineColorBlendStateCreateInfo& colorBlend,
    vk::PipelineLayout layout, vk::RenderPass renderPass) {
    return pipelines_.Emplace(std::make_unique<GraphicsPipeline>(
        this, vertexLayout, inputAsm, shaders, viewport, raster, depthStencil,
        multisample, colorBlend, layout, renderPass));
}

ResResult<RenderPass> Device::CreateRenderPass(
    const std::vector<vk::AttachmentDescription>& attachmentDescs,
    const std::vector<vk::SubpassDescription>& subpasses,
    const std::vector<vk::SubpassDependency>& subpassDeps) {
    return renderPasses_.Emplace(std::make_unique<RenderPass>(
        this, attachmentDescs, subpasses, subpassDeps));
}

ResResult<CommandPool> Device::CreateCommandPool(
    vk::CommandPoolCreateFlags flag, uint32_t queueIndex) {
    return cmdPools_.Emplace(
        std::make_unique<CommandPool>(this, flag, queueIndex));
}

ResResult<Semaphore> Device::CreateSemaphore() {
    return semaphores_.Emplace(std::make_unique<Semaphore>(this));
}

ResResult<Fence> Device::CreateFence(bool signaled) {
    return fences_.Emplace(std::make_unique<Fence>(this, signaled));
}

ResResult<Event> Device::CreateEvent(bool deviceOnly) {
    return events_.Emplace(std::make_unique<Event>(this, deviceOnly));
}

ResResult<DescriptorSetLayout> Device::CreateDescriptorSetLayout(
    const std::vector<vk::DescriptorSetLayoutBinding>& bindings) {
    return descriptorSetLayouts_.Emplace(
        std::make_unique<DescriptorSetLayout>(this, bindings));
}

ResResult<DescriptorPool> Device::CreateDescriptorPool(
    const std::vector<vk::DescriptorPoolSize>& sizes, uint32_t maxSetCount) {
    return descriptorPools_.Emplace(
        std::make_unique<DescriptorPool>(this, sizes, maxSetCount));
}

ResResult<PipelineLayout> Device::CreatePipelineLayout(
    const std::vector<vk::DescriptorSetLayout>& layouts,
    const std::vector<vk::PushConstantRange>& pushConstantRanges) {
    return pipelineLayouts_.Emplace(
        std::make_unique<PipelineLayout>(this, layouts, pushConstantRanges));
}

ResResult<Framebuffer> Device::CreateFramebuffer(
        const std::vector<vk::ImageView>& views, uint32_t width,
        uint32_t height, uint32_t layers, vk::RenderPass renderPass) {
    return framebuffers_.Emplace(std::make_unique<Framebuffer>(
        this, views, width, height, layers, renderPass));
}

ResResult<Sampler> Device::CreateSampler(
        vk::Filter min, vk::Filter mag, vk::SamplerMipmapMode mipmap,
        vk::SamplerAddressMode u, vk::SamplerAddressMode v,
        vk::SamplerAddressMode w, float mipLodBias, bool anisotropyEnable,
        float maxAnisotropy, bool compareEnable, vk::CompareOp compare,
        float minLod, float maxLod, vk::BorderColor borderColor,
        bool unnormalizedCoordinates) {
    return samplers_.Emplace(std::make_unique<Sampler>(
        this, min, mag, mipmap, u, v, w, mipLodBias, anisotropyEnable,
        maxAnisotropy, compareEnable, compare, minLod, maxLod, borderColor,
        unnormalizedCoordinates));
}

}  // namespace nickel::vulkan