#include "rhi/vulkan/sync.hpp"
#include "rhi/vulkan/device.hpp"

namespace nickel::rhi::vulkan {

Semaphore::Semaphore(Device* device): device_{device} {
    vk::SemaphoreCreateInfo createInfo;
    sem_ = device->Raw().createSemaphore(createInfo);
    if (!sem_) {
        LOGE(log_tag::Vulkan, "create semaphore failed");
    }
}

Semaphore::~Semaphore() {
    if (device_ && sem_) {
        device_->Raw().destroySemaphore(sem_);
    }
}

Fence::Fence(Device* device, bool signaled): device_{device} {
    vk::FenceCreateInfo createInfo;
    if (signaled) {
        createInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    }

    fence_ = device->Raw().createFence(createInfo);
    if (!fence_) {
        LOGE(log_tag::Vulkan, "create fence failed");
    }
}

Fence::~Fence() {
    if (device_ && fence_) {
        device_->Raw().destroyFence(fence_);
    }
}

Event::Event(Device* device, bool deviceOnly): device_{device} {
    vk::EventCreateInfo createInfo;
    if (deviceOnly) {
        createInfo.setFlags(vk::EventCreateFlagBits::eDeviceOnly);
    }
    event_ = device->Raw().createEvent(createInfo);
    if (!event_) {
        LOGE(log_tag::Vulkan, "create event failed");
    }
}

}