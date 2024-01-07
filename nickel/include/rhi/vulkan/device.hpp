#pragma once

#include "pch.hpp"
#include "rhi/vulkan/instance.hpp"
#include "rhi/device.hpp"

namespace nickel::rhi::vulkan {

class Surface;

class Device: public rhi::Device {
public:
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsIndex;
        std::optional<uint32_t> presentIndex;

        explicit operator bool() const {
            return graphicsIndex && presentIndex;
        }
    };

    Device(Instance&, Surface*);
    ~Device() {
        device_.destroy();
    }

    vk::Device& Raw() { return device_; }

    auto& GetInstance() const { return instance_; }
    auto& GetInstance() { return instance_; }

    auto& GetPhysicalDevice() const { return phyDevice_; }
    auto& GetPhysicalDevice() { return phyDevice_; }

    auto& GetQueueIndices() const { return queueIndices_; }

private:
    vk::Device device_;
    vk::Queue graphicsQueue_;
    vk::Queue presentQueue_;
    Instance& instance_;
    vk::PhysicalDevice phyDevice_;
    QueueFamilyIndices queueIndices_;

    void createDevice(Instance& instance, Surface*);

    size_t choosePhysicalDevice(const std::vector<vk::PhysicalDevice>&);
    QueueFamilyIndices chooseQueue(
        vk::PhysicalDevice phyDevice,
        Surface* surface,
        const std::vector<vk::QueueFamilyProperties>&);
};

}