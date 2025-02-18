#include "nickel/graphics/lowlevel/internal/adapter_impl.hpp"
#include "nickel/common/assert.hpp"
#include "nickel/common/log.hpp"
#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"
#include "nickel/internal/pch.hpp"
#include "nickel/video/internal/window_impl.hpp"

namespace nickel::graphics {

static VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT level,
                    VkDebugUtilsMessageTypeFlagsEXT type,
                    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                    void* user_data) {
    std::string type_name;

    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        type_name += "General";
    }
    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        type_name += type_name.empty() ? "Validation" : "|Validation";
    }
    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        type_name += type_name.empty() ? "Performance" : "|Performance";
    }
    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT) {
        type_name += type_name.empty() ? "DeviceAddressBinding"
                                       : "|DeviceAddressBinding";
    }

    switch (level) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LOGT("[Vulkan][{}]: {}", type_name, callback_data->pMessage);
            return false;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOGI("[Vulkan][{}]: {}", type_name, callback_data->pMessage);
            return false;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOGW("[Vulkan][{}]: {}", type_name, callback_data->pMessage);
            return false;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOGE("[Vulkan][{}]: {}", type_name, callback_data->pMessage);
            return true;
    }

    NICKEL_CANT_REACH();
    return false;
}

AdapterImpl::AdapterImpl(const video::Window::Impl& window) {
    if (volkInitialize() != VK_SUCCESS) {
        LOGE("volk init failed");
    }

    LOGI("creating vulkan instance");
    createInstance();

    LOGI("picking up physics device");
    pickupPhysicalDevice();
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(m_phy_device, &props);
    LOGI("pick {}", props.deviceName);

    queryLimits();

    LOGI("creating surface");
    createSurface(window);

    LOGI("creating render device");
    createDevice(window.GetSize());
}

void AdapterImpl::createInstance() {
    VkInstanceCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_API_VERSION_1_3;
    appInfo.pEngineName = "NickelEngine";
    appInfo.pApplicationName = "NickelEngine";
    ci.pApplicationInfo = &appInfo;

    std::vector<const char*> require_extensions;
    unsigned int count;
    auto sdl_required_extensions = SDL_Vulkan_GetInstanceExtensions(&count);

    for (int i = 0; i < count; i++) {
        require_extensions.push_back(sdl_required_extensions[i]);
    }
    require_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    uint32_t extension_count;
    VK_CALL(vkEnumerateInstanceExtensionProperties(nullptr, &extension_count,
                                                   nullptr));
    std::vector<VkExtensionProperties> support_extensions;
    support_extensions.resize(extension_count);
    VK_CALL(vkEnumerateInstanceExtensionProperties(nullptr, &extension_count,
                                                   support_extensions.data()));

    using LiteralString = const char*;
    RemoveUnexistsElems<LiteralString, VkExtensionProperties>(
        require_extensions, support_extensions,
        [](const LiteralString& require, const VkExtensionProperties& prop) {
            return std::strcmp(prop.extensionName, require) == 0;
        });

    ci.enabledExtensionCount = require_extensions.size();
    ci.ppEnabledExtensionNames = require_extensions.data();

    std::vector<VkLayerProperties> support_layers;
    uint32_t layer_count;
    VK_CALL(vkEnumerateInstanceLayerProperties(&layer_count, nullptr));
    support_layers.resize(layer_count);
    VK_CALL(
        vkEnumerateInstanceLayerProperties(&layer_count, support_layers.data()));

    std::vector<const char*> require_layers;
#ifdef NICKEL_DEBUG
    require_layers.push_back("VK_LAYER_KHRONOS_validation");
    LOGI("Vulkan enable validation layer");

    VkDebugUtilsMessengerCreateInfoEXT debug_ci{};
    debug_ci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_ci.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_ci.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_ci.pfnUserCallback = VulkanDebugCallback;
    debug_ci.pUserData = nullptr;  // Optional
    ci.pNext = &debug_ci;
#endif

    RemoveUnexistsElems<const char*, VkLayerProperties>(
        require_layers, support_layers,
        [](const LiteralString& require, const VkLayerProperties& prop) {
            return std::strcmp(prop.layerName, require) == 0;
        });

    ci.enabledLayerCount = require_layers.size();
    ci.ppEnabledLayerNames = require_layers.data();
    VK_CALL(vkCreateInstance(&ci, nullptr, &m_instance));
    volkLoadInstance(m_instance);

#ifdef NICKEL_DEBUG
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        m_instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(m_instance, &debug_ci, nullptr, &m_debug_utils_messenger);
    } else {
        LOGE("vkCreateDebugUtilsMessengerEXT function not exists");
    }
#endif
}

void AdapterImpl::pickupPhysicalDevice() {
    NICKEL_ASSERT(m_instance, "vulkan instance not create");

    std::vector<VkPhysicalDevice> physical_devices;
    uint32_t count;
    VK_CALL(vkEnumeratePhysicalDevices(m_instance, &count, nullptr));
    physical_devices.resize(count);
    VK_CALL(
        vkEnumeratePhysicalDevices(m_instance, &count, physical_devices.data()));

    NICKEL_ASSERT(!physical_devices.empty(), "no vulkan physics device");
    m_phy_device = physical_devices[0];
}

void AdapterImpl::createSurface(const video::Window::Impl& impl) {
    SDL_Vulkan_CreateSurface(impl.m_window, m_instance, nullptr, &m_surface);

    if (!m_surface) {
        LOGC("create vulkan surface failed");
    }
}

void AdapterImpl::createDevice(const SVector<uint32_t, 2>& window_size) {
    m_device = new DeviceImpl{*this, window_size};
}

void AdapterImpl::queryLimits() {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(m_phy_device, &props);

    m_limits.min_uniform_buffer_offset_alignment =
        props.limits.minUniformBufferOffsetAlignment;
}

AdapterImpl::~AdapterImpl() {
    delete m_device;
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

    auto vkDestroyDebugUtilsMessengerExTFuc = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        m_instance, "vkDestroyDebugUtilsMessengerEXT");
    if (vkDestroyDebugUtilsMessengerExTFuc != nullptr) {
        vkDestroyDebugUtilsMessengerExTFuc(m_instance, m_debug_utils_messenger, nullptr);
    }

    vkDestroyInstance(m_instance, nullptr);
}

Device AdapterImpl::GetDevice() const {
    return Device{m_device};
}

}  // namespace nickel::graphics