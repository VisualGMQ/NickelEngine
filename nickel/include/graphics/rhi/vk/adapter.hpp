#pragma once

#include "graphics/rhi/impl/adapter.hpp"
#include "graphics/rhi/vk/pch.hpp"

namespace nickel::rhi::vulkan {

class AdapterImpl: public rhi::AdapterImpl {
public:
    explicit AdapterImpl(void* window);
    ~AdapterImpl();

    GPUSupportFeatures Features() override;
    const GPUSupportLimits& Limits() const override;
    Device RequestDevice() override;
    Adapter::Info RequestAdapterInfo() const override;

    vk::Instance instance;
    vk::PhysicalDevice phyDevice;
    vk::SurfaceKHR surface;
    void* window;

private:
    Adapter::Info info_;
    GPUSupportLimits limits_;

    void createInstance(void* window);
    void pickupPhyDevice();
    void createSurface(void* window);
    void fillAdapterInfo();

    void querySupportLimits();
};

}