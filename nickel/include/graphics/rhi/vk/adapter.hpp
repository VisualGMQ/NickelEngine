#pragma once

#include "graphics/rhi/impl/adapter.hpp"
#include "graphics/rhi/vk/pch.hpp"

namespace nickel::rhi::vulkan {

class AdapterImpl: public rhi::AdapterImpl {
public:
    explicit AdapterImpl(void* window);
    ~AdapterImpl();

    GPUSupportFeatures Features() override;
    GPUSupportLimits Limits() override;
    Device RequestDevice() override;
    Adapter::Info RequestAdapterInfo() override;

    vk::Instance instance;
    vk::PhysicalDevice phyDevice;
    vk::SurfaceKHR surface;
    void* window;

private:
    Adapter::Info info_;

    void createInstance(void* window);
    void pickupPhyDevice();
    void createSurface(void* window);
    void fillAdapterInfo();
};

}