#include "vulkan/buffer.hpp"
#include "vulkan/device.hpp"
#include "vulkan/util.hpp"

namespace nickel::vulkan {

Buffer::Buffer(Device* device, uint64_t size, vk::BufferUsageFlags usage,
               vk::MemoryPropertyFlags flags,
               std::vector<uint32_t> queueIndices)
    : device_{device}, size_{size}, usage_{usage}, prop_{flags} {
    createBuffer(size, usage, queueIndices);
    allocateMem(buffer_, flags);
    VK_CALL_NO_VALUE(device->GetDevice().bindBufferMemory(buffer_, mem_, 0));
}

Buffer::~Buffer() {
    if (device_) {
        if (mem_) {
            device_->GetDevice().freeMemory(mem_);
        }
        if (buffer_) {
            device_->GetDevice().destroyBuffer(buffer_);
        }
    }
}

void* Buffer::Map(uint64_t offset, uint64_t size) {
    void* map{};
    VK_CALL(map, device_->GetDevice().mapMemory(mem_, offset, size));
    return map;
}

void Buffer::Unmap() {
    device_->GetDevice().unmapMemory(mem_);
}

void Buffer::createBuffer(uint64_t size, vk::BufferUsageFlags usage,
                          const std::vector<uint32_t>& queueIndices) {
    vk::BufferCreateInfo createInfo;
    createInfo.setSize(size).setUsage(usage);
    if (queueIndices.size() > 1) {
        createInfo.setQueueFamilyIndices(queueIndices);
        createInfo.setSharingMode(vk::SharingMode::eConcurrent);
    } else {
        createInfo.setSharingMode(vk::SharingMode::eExclusive);
    }

    VK_CALL(buffer_, device_->GetDevice().createBuffer(createInfo));
}

std::optional<uint32_t> Buffer::findMemoryType(
    const vk::MemoryRequirements& requirements, vk::MemoryPropertyFlags prop) {
    auto properties = device_->GetPhyDevice().getMemoryProperties();
    for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
        if (((1 << i) & requirements.memoryTypeBits) &&
            (properties.memoryTypes[i].propertyFlags & prop) == prop) {
            return i;
        }
    }
    return {};
}

void Buffer::allocateMem(vk::Buffer buffer, vk::MemoryPropertyFlags flags) {
    auto requirements =
        device_->GetDevice().getBufferMemoryRequirements(buffer);
    auto type = findMemoryType(requirements, flags);

    if (!type) {
        LOGE(log_tag::Vulkan, "find corresponding memory type failed");
    } else {
        vk::MemoryAllocateInfo allocInfo;
        allocInfo.setAllocationSize(requirements.size)
            .setMemoryTypeIndex(type.value());
        VK_CALL(mem_, device_->GetDevice().allocateMemory(allocInfo));
    }
}

}  // namespace nickel::vulkan