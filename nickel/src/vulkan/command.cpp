#include "vulkan/command.hpp"
#include "vulkan/device.hpp"
#include "vulkan/util.hpp"

namespace nickel::vulkan {

CommandPool::CommandPool(Device* device, vk::CommandPoolCreateFlags flag,
                         uint32_t queueIndex)
    : device_{device} {
    vk::CommandPoolCreateInfo createInfo;
    createInfo.setQueueFamilyIndex(queueIndex).setFlags(flag);
    VK_CALL(pool_, device->GetDevice().createCommandPool(createInfo));
}

CommandPool::~CommandPool() {
    if (device_ && pool_) {
        device_->GetDevice().destroyCommandPool(pool_);
    }
}

std::vector<vk::CommandBuffer> CommandPool::Allocate(
    vk::CommandBufferLevel level, uint32_t count) {
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.setCommandPool(pool_).setCommandBufferCount(count).setLevel(
        level);
    std::vector<vk::CommandBuffer> cmds;
    VK_CALL(cmds, device_->GetDevice().allocateCommandBuffers(allocInfo));
    return cmds;
}

void CommandPool::Reset() {
    device_->GetDevice().resetCommandPool(pool_);
}

}  // namespace nickel::vulkan