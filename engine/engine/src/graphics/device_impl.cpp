#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/common/log.hpp"
#include "nickel/graphics/internal/adapter_impl.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

Device::Impl::Impl(const Adapter::Impl& impl) {
    m_queue_indices = chooseQueue(impl.m_phyDevice, impl.m_surface);

    if (!m_queue_indices) {
        LOGC("no graphics queue in your GPU");
    }

    VkDeviceCreateInfo device_ci{};
    device_ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::set indices{m_queue_indices.graphicsIndex.value(),
                     m_queue_indices.presentIndex.value()};

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

    std::vector requireExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    std::vector<VkExtensionProperties> extension_props;
    uint32_t extensionCount = 0;
    VK_CALL(vkEnumerateDeviceExtensionProperties(impl.m_phyDevice, nullptr, &extensionCount, nullptr));
    extension_props.resize(extensionCount); 
    VK_CALL(vkEnumerateDeviceExtensionProperties(impl.m_phyDevice, nullptr, &extensionCount, extension_props.data()));

    RemoveUnexistsElems<const char*, VkExtensionProperties>(
        requireExtensions, extension_props,
        [](const auto& e1, const VkExtensionProperties& e2) {
            return std::strcmp(e1, e2.extensionName) == 0;
        });

    for (auto ext : requireExtensions) {
        LOGI("enable vulkan device extension: {}", ext);
    }

    std::vector<const char*> extension_names;
    for (auto ext : extension_props) {
        extension_names.push_back(ext.extensionName);
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

    vkGetDeviceQueue(m_device, m_queue_indices.graphicsIndex.value(), 0,
                     &m_graphics_queue);
    vkGetDeviceQueue(m_device, m_queue_indices.presentIndex.value(), 0,
                     &m_present_queue);
}

Device::Impl::QueueFamilyIndices Device::Impl::chooseQueue(
    VkPhysicalDevice phyDevice, VkSurfaceKHR surface) {
    uint32_t count = 0;
    std::vector<VkQueueFamilyProperties> queue_families;
    vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &count, nullptr);
    queue_families.resize(count);
    vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &count, queue_families.data());

    QueueFamilyIndices indices;

    for (int i = 0; i < queue_families.size(); i++) {
        auto& prop = queue_families[i];
        if (prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsIndex = i;
        }
        VkBool32 supportSurface = false;
        VK_CALL(vkGetPhysicalDeviceSurfaceSupportKHR(phyDevice, i, surface, &supportSurface));
        if (supportSurface) {
            indices.presentIndex = i;
        }

        if (indices) {
            break;
        }
    }

    return indices;
}

Device::Impl::~Impl() {
    vkDestroyDevice(m_device, nullptr);
}

}

