#pragma once

#include "graphics/rhi/vk/util.hpp"
#include "graphics/rhi/vk/pch.hpp"
#include "graphics/rhi/impl/queue.hpp"
#include "graphics/rhi/command.hpp"

namespace nickel::rhi::vulkan {

class DeviceImpl;

class QueueImpl : public rhi::QueueImpl {
public:
    explicit QueueImpl(DeviceImpl& dev, vk::Queue);

    void Submit(const std::vector<CommandBuffer>&) override;

    vk::Queue queue;

private:
    DeviceImpl& dev_;
};

}  // namespace nickel::rhi::vulkan