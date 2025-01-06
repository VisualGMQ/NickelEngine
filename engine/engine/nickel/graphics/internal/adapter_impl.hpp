#pragma once
#include "nickel/common/math/smatrix.hpp"
#include "nickel/graphics/adapter.hpp"
#include "nickel/video/window.hpp"

namespace nickel::graphics {

class Device;
class AdapterImpl;
class DeviceImpl;

class AdapterImpl {
public:
    AdapterImpl(const video::Window::Impl& window);
    AdapterImpl(const AdapterImpl&) = delete;
    AdapterImpl& operator=(const AdapterImpl&) = delete;
    ~AdapterImpl();
    Device GetDevice() const;

    VkInstance m_instance;
    VkPhysicalDevice m_phyDevice;
    VkSurfaceKHR m_surface;
    DeviceImpl* m_device{};

private:
    void createInstance();
    void pickupPhysicalDevice();
    void createSurface(const video::Window::Impl& impl);
    void createDevice(const SVector<uint32_t, 2>& window_size);
};

}  // namespace nickel::graphics