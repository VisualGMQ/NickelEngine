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

    VkInstance m_instance;
    VkPhysicalDevice m_phyDevice;
    VkSurfaceKHR m_surface;
    DeviceImpl* m_device{};
    Adapter::Limits m_limits;
    VkDebugUtilsMessengerEXT m_debug_utils_messenger;

private:
    void createInstance();
    void pickupPhysicalDevice();
    void createSurface(const video::Window::Impl& impl);
    void createDevice(const SVector<uint32_t, 2>& window_size);
    void queryLimits();
};

}  // namespace nickel::graphics