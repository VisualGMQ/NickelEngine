#include "nickel/graphics/internal/adapter_impl.hpp"
#include "nickel/common/assert.hpp"
#include "nickel/common/log.hpp"
#include "nickel/graphics/device.hpp"
#include "nickel/graphics/internal/vk_call.hpp"
#include "nickel/internal/pch.hpp"
#include "nickel/video/internal/window_impl.hpp"

namespace nickel::graphics {

AdapterImpl::AdapterImpl(const video::Window::Impl& window) {
    if (volkInitialize() != VK_SUCCESS) {
        LOGE("volk init failed");
    }

    LOGI("creating vulkan instance");
    createInstance();

    LOGI("picking up physics device");
    pickupPhysicalDevice();
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(m_phyDevice, &props);
    LOGI("pick {}", props.deviceName);

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
    ci.pApplicationInfo = &appInfo;

    unsigned int count;
    auto extensions = SDL_Vulkan_GetInstanceExtensions(&count);

    ci.enabledExtensionCount = count;
    ci.ppEnabledExtensionNames = extensions;

    std::vector<VkLayerProperties> supportLayers;
    uint32_t layerCount;
    VK_CALL(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
    supportLayers.resize(layerCount);
    VK_CALL(
        vkEnumerateInstanceLayerProperties(&layerCount, supportLayers.data()));

    std::vector<const char*> requireLayers;
#ifdef NICKEL_DEBUG
    requireLayers.push_back("VK_LAYER_KHRONOS_validation");
    LOGI("Vulkan enable validation layer");
#endif

    using LiteralString = const char*;
    RemoveUnexistsElems<const char*, VkLayerProperties>(
        requireLayers, supportLayers,
        [](const LiteralString& require, const VkLayerProperties& prop) {
            return std::strcmp(prop.layerName, require) == 0;
        });

    ci.enabledLayerCount = requireLayers.size();
    ci.ppEnabledLayerNames = requireLayers.data();
    vkCreateInstance(&ci, nullptr, &m_instance);
    volkLoadInstance(m_instance);
}

void AdapterImpl::pickupPhysicalDevice() {
    NICKEL_ASSERT(m_instance, "vulkan instance not create");

    std::vector<VkPhysicalDevice> physicalDevices;
    uint32_t count;
    VK_CALL(vkEnumeratePhysicalDevices(m_instance, &count, nullptr));
    physicalDevices.resize(count);
    VK_CALL(
        vkEnumeratePhysicalDevices(m_instance, &count, physicalDevices.data()));

    NICKEL_ASSERT(!physicalDevices.empty(), "no vulkan physics device");
    m_phyDevice = physicalDevices[0];
}

void AdapterImpl::createSurface(const video::Window::Impl& impl) {
    SDL_Vulkan_CreateSurface(impl.m_window, m_instance, nullptr, &m_surface);

    if (!m_surface) {
        LOGC("create vulkan surface failed");
    }
}

void AdapterImpl::createDevice(const SVector<uint32_t, 2>& window_size) {
    m_device = new Device{*this, window_size};
}

AdapterImpl::~AdapterImpl() {
    delete m_device;
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkDestroyInstance(m_instance, nullptr);
}

}  // namespace nickel::graphics