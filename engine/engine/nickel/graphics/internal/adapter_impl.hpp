#pragma once
#include "nickel/graphics/adapter.hpp"
#include "nickel/video/window.hpp"

namespace nickel::graphics {
class Device;

class Adapter::Impl {
public:
    Impl(const video::Window::Impl& window);
    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;
    ~Impl();

    VkInstance m_instance;
    VkPhysicalDevice m_phyDevice;
    VkSurfaceKHR m_surface;
    Device* m_device{};

private:
    void createInstance();
    void pickupPhysicalDevice();
    void createSurface(const video::Window::Impl& impl);
    void createDevice();
};

}  // namespace nickel::graphics