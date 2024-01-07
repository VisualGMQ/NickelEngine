#include "rhi/vulkan/util.hpp"
#include "rhi/vulkan/device.hpp"
#include "rhi/vulkan/surface.hpp"

namespace nickel::rhi::vulkan {

int getDeviceTypeScore(vk::PhysicalDeviceType type) {
    switch(type) {
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

Device::Device(Instance& instance, Surface* surface): instance_(instance) {
    createDevice(instance, surface);
}

void Device::createDevice(Instance& instance, Surface* surface) {
    auto& inst = instance.Raw();
    auto phyDevices = inst.enumeratePhysicalDevices();

    size_t chooseDevice = choosePhysicalDevice(phyDevices);
    phyDevice_ = phyDevices[chooseDevice];

    LOGW(nickel::log_tag::Vulkan, "choose GPU ",
         phyDevice_.getProperties().deviceName);

    queueIndices_ = chooseQueue(phyDevice_, surface, phyDevice_.getQueueFamilyProperties());

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

    std::vector<const char*> requireExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    auto existsExtensions = phyDevice_.enumerateDeviceExtensionProperties();

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

    device_ = phyDevice_.createDevice(createInfo);
    graphicsQueue_ = device_.getQueue(queueIndices_.graphicsIndex.value(), 0);
    presentQueue_ = device_.getQueue(queueIndices_.presentIndex.value(), 0);
}

size_t Device::choosePhysicalDevice(
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

    return chooseDevice;
}

Device::QueueFamilyIndices Device::chooseQueue(
    vk::PhysicalDevice phyDevice,
    Surface* surface,
    const std::vector<vk::QueueFamilyProperties>& props) {
    auto queueProps = phyDevice.getQueueFamilyProperties();

    QueueFamilyIndices indices;

    for (int i = 0; i < queueProps.size(); i++) {
        auto& prop = queueProps[i];
        if (prop.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphicsIndex = i;
        }
        if (phyDevice.getSurfaceSupportKHR(i, surface->Raw())) {
            indices.presentIndex = i;
        }

        if (indices) {
            break;
        }
    }

    return indices;
}



}  // namespace nickel::rhi::vulkan