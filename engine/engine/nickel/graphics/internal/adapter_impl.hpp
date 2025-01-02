#pragma once
#include "nickel/graphics/adapter.hpp"
#include "nickel/video/window.hpp"
#include "nickel/common/math/smatrix.hpp"

namespace nickel::graphics {

class Device;
class AdapterImpl;

class AdapterImpl {
public:
    AdapterImpl(const video::Window::Impl& window);
    AdapterImpl(const AdapterImpl&) = delete;
    AdapterImpl& operator=(const AdapterImpl&) = delete;
    ~AdapterImpl();

    VkInstance m_instance;
    VkPhysicalDevice m_phyDevice;
    VkSurfaceKHR m_surface;
    Device* m_device{};

private:
    void createInstance();
    void pickupPhysicalDevice();
    void createSurface(const video::Window::Impl& impl);
    void createDevice(const SVector<uint32_t, 2>& window_size);
};

}  // namespace nickel::graphics