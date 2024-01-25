#include "vulkan/sync.hpp"
#include "vulkan/device.hpp"
#include "vulkan/util.hpp"

namespace nickel::vulkan {

Semaphore::Semaphore(Device* device) : device_{device} {
    vk::SemaphoreCreateInfo info;
    VK_CALL(sem_, device_->GetDevice().createSemaphore(info));
}

Semaphore::~Semaphore() {
    if (device_) {
        device_->GetDevice().destroySemaphore(sem_);
    }
}

Fence::Fence(Device* device, bool signal) : device_{device} {
    vk::FenceCreateInfo info;
    if (signal) {
        info.setFlags(vk::FenceCreateFlagBits::eSignaled);
    }
    VK_CALL(fence_, device_->GetDevice().createFence(info));
}

Fence::~Fence() {
    if (device_) {
        device_->GetDevice().destroyFence(fence_);
    }
}

Event::Event(Device* device, bool deviceOnly) : device_{device} {
    vk::EventCreateInfo info;
    if (deviceOnly) {
        info.setFlags(vk::EventCreateFlagBits::eDeviceOnly);
    }
    VK_CALL(event_, device_->GetDevice().createEvent(info));
}

Event::~Event() {
    if (device_) {
        device_->GetDevice().destroyEvent(event_);
    }
}

}  // namespace nickel::vulkan