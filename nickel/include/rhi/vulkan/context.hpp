#pragma once

#include "rhi/context.hpp"
#include "rhi/vulkan/instance.hpp"
#include "rhi/vulkan/device.hpp"

namespace nickel::rhi::vulkan {

class Context final: public rhi::Context {
public:
    const class Instance& Instance() const override { return *instance_; }
    const class Device& Device() const override { return *device_; }

private:
    std::unique_ptr<vulkan::Instance> instance_;
    std::unique_ptr<vulkan::Device> device_;
};

inline void SetAPIHint() {}

}