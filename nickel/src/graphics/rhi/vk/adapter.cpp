#include "graphics/rhi/vk/adapter.hpp"
#include "common/log.hpp"
#include "common/log_tag.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/util.hpp"

namespace nickel::rhi::vulkan {

AdapterImpl::AdapterImpl(void* window) : window{window} {
    createInstance(window);
    createSurface(window);
    pickupPhyDevice();
    fillAdapterInfo();
    querySupportLimits();
}

void AdapterImpl::createInstance(void* window) {
    vk::InstanceCreateInfo createInfo;
    vk::ApplicationInfo appInfo;
    appInfo.setApiVersion(VK_API_VERSION_1_3);
    appInfo.setPEngineName("NickelEngine");
    createInfo.setPApplicationInfo(&appInfo);

    unsigned int count;
    SDL_Window* sdlWindow = (SDL_Window*)window;
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
    VK_CALL(instance, vk::createInstance(createInfo));
}

int getPhyDeviceTypeScore(vk::PhysicalDeviceType type) {
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

void AdapterImpl::pickupPhyDevice() {
    std::vector<vk::PhysicalDevice> phyDevices;
    VK_CALL(phyDevices, instance.enumeratePhysicalDevices());
    int chooseDevice = 0;
    int score = 0;
    for (int i = 0; i < phyDevices.size(); i++) {
        auto& phyDevice = phyDevices[i];
        auto prop = phyDevice.getProperties();
        auto feature = phyDevice.getFeatures();

        int curScore = getPhyDeviceTypeScore(prop.deviceType);
        if (curScore > score) {
            score = curScore;
            chooseDevice = i;
        }
    }

    phyDevice = phyDevices[chooseDevice];

    auto props = phyDevice.getProperties();
    LOGW(log_tag::Vulkan, "pickup ", props.deviceName.data());
}

void AdapterImpl::createSurface(void* window) {
    VkSurfaceKHR surface;
    SDL_Vulkan_CreateSurface((SDL_Window*)window, instance, &surface);

    if (!surface) {
        LOGW(nickel::log_tag::Vulkan, "create surface failed");
    }
    this->surface = surface;
}

void AdapterImpl::fillAdapterInfo() {
    auto prop = phyDevice.getProperties();
    info_.device = prop.deviceName.data();
    info_.api = APIPreference::Vulkan;
    info_.vendorID = prop.vendorID;
}

AdapterImpl::~AdapterImpl() {
    instance.destroySurfaceKHR(surface);
    instance.destroy();
}

GPUSupportFeatures AdapterImpl::Features() {
    // TODO: not finish
    return {};
}

const GPUSupportLimits& AdapterImpl::Limits() const {
    return limits_;
}

void AdapterImpl::querySupportLimits() {
    auto limits = phyDevice.getProperties().limits;
    limits_.nonCoherentAtomSize = limits.nonCoherentAtomSize;
    limits_.minUniformBufferOffsetAlignment = limits.minUniformBufferOffsetAlignment;
    limits_.minStorageBufferOffsetAlignment = limits.minStorageBufferOffsetAlignment;
    limits_.maxPushConstantSize = limits.maxPushConstantsSize;
}

Device AdapterImpl::RequestDevice() {
    return Device(*this);
}

Adapter::Info AdapterImpl::RequestAdapterInfo() {
    return info_;
}

}  // namespace nickel::rhi::vulkan