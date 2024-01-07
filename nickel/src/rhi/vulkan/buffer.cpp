#include "rhi/vulkan/buffer.hpp"
#include "rhi/vulkan/device.hpp"
#include "rhi/vulkan/enum_convert.hpp"

namespace nickel::rhi::vulkan {

Buffer::Buffer(
    Device* device, uint64_t size, BufferUsageFlags flags,
    std::optional<std::reference_wrapper<std::vector<uint32_t>>> queueIndices)
    : device_(device) {
    vk::BufferCreateInfo createInfo;
    createInfo.setSize(size).setUsage(BufferUsageFlags2Vk(flags));
    if (!queueIndices->get().empty()) {
        createInfo.setQueueFamilyIndices(queueIndices.value().get());
        createInfo.setSharingMode(vk::SharingMode::eConcurrent);
    }

    buffer_ = device_->Raw().createBuffer(createInfo);
    if (!buffer_) {
        LOGE(log_tag::Vulkan, "create buffer failed");
    }
}

Buffer::~Buffer() {
    if (device_ && buffer_) {
        device_->Raw().destroyBuffer(buffer_);
    }

    vk::MemoryPropertyFlagBits b;
}

DeviceMemory::DeviceMemory(Device* device, const Buffer& buffer,
                           MemoryPropertyFlags flags)
    : device_(device) {
    auto requirements = device->Raw().getBufferMemoryRequirements(buffer.Raw());
    auto type = findMemoryType(requirements, MemoryPropertyFlags2Vk(flags));

    if (!type) {
        LOGE(log_tag::Vulkan, "find corresponding memory type failed");
    } else {
        vk::MemoryAllocateInfo allocInfo;
        allocInfo.setAllocationSize(requirements.size)
            .setMemoryTypeIndex(type.value());
        mem_ = device->Raw().allocateMemory(allocInfo);
    }
}

std::optional<uint32_t> DeviceMemory::findMemoryType(
    const vk::MemoryRequirements& requirements, vk::MemoryPropertyFlags prop) {
    auto properties = device_->GetPhysicalDevice().getMemoryProperties();
    for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
        if (((1 << i) & requirements.memoryTypeBits) &&
            (properties.memoryTypes[i].propertyFlags & prop) == prop) {
            return i;
        }
    }
    return {};
}

DeviceMemory::~DeviceMemory() {
    if (device_ && mem_) {
        device_->Raw().freeMemory(mem_);
    }
}

MemoryGuard DeviceMemory::Map(uint64_t offset, uint64_t size) {
    return {*this, map(offset, size)};
}

void* DeviceMemory::map(uint64_t offset, uint64_t size) {
    return device_->Raw().mapMemory(mem_, offset, size);
}

void DeviceMemory::unmap() {
    device_->Raw().unmapMemory(mem_);
}

MemoryGuard::MemoryGuard(DeviceMemory& mem, void* memory)
    : mem_{&mem}, memory{memory} {}

MemoryGuard::MemoryGuard(DeviceMemory& mem, uint64_t offset, uint64_t size)
    : mem_{&mem} {
    memory = mem_->map(offset, size);
}

MemoryGuard::~MemoryGuard() {
    Unmap();
}

void MemoryGuard::Unmap() {
    if (memory) {
        mem_->unmap();
        memory = nullptr;
    }
}

}  // namespace nickel::rhi::vulkan