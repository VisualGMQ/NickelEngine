#pragma once

#include "vulkan/pch.hpp"
#include "stdpch.hpp"

namespace nickel::vulkan {

class Device;

class CommandPool {
public:
    CommandPool(Device* device, vk::CommandPoolCreateFlags flag,
                uint32_t queueIndex);
    CommandPool(const CommandPool&) = delete;
    CommandPool& operator=(const CommandPool&) = delete;

    CommandPool(CommandPool&& o) { swap(*this, o); }

    CommandPool& operator=(CommandPool&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    ~CommandPool();

    std::vector<vk::CommandBuffer> Allocate(vk::CommandBufferLevel,
                                            uint32_t count);
    void Reset();

private:
    Device* device_{};
    vk::CommandPool pool_;

    friend void swap(CommandPool& o1, CommandPool& o2) noexcept {
        using std::swap;
        swap(o1.device_, o2.device_);
        swap(o1.pool_, o2.pool_);
    }
};

}  // namespace nickel::vulkan