#pragma once
#include "nickel/common/math/smatrix.hpp"
#include "nickel/graphics/lowlevel/adapter.hpp"
#include "nickel/video/window.hpp"

namespace nickel::graphics {

class Device;
class AdapterImpl;
class DeviceImpl;

class AdapterImpl {
public:
    AdapterImpl(const video::Window::Impl& window);
    AdapterImpl(const AdapterImpl&) = delete;
    AdapterImpl(AdapterImpl&&) = delete;
    AdapterImpl& operator=(const AdapterImpl&) = delete;
    AdapterImpl& operator=(AdapterImpl&&) = delete;

    ~AdapterImpl();
    Device GetDevice() const;

    const Adapter::Limits& GetLimits() const { return m_limits; }

    VkInstance m_instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_phy_device = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    DeviceImpl* m_device{};
    Adapter::Limits m_limits;
    VkDebugUtilsMessengerEXT m_debug_utils_messenger;
    
    void CreateSurface(const video::Window::Impl& impl);

private:
    void createInstance();
    void pickupPhysicalDevice();
    void createDevice(const SVector<uint32_t, 2>& window_size);
    void queryLimits();
};

}  // namespace nickel::graphics