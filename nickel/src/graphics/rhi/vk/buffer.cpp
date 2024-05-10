#include "graphics/rhi/vk/buffer.hpp"
#include "graphics/rhi/vk/convert.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/memory.hpp"

namespace nickel::rhi::vulkan {

BufferImpl::BufferImpl(DeviceImpl& dev, vk::PhysicalDevice phyDev,
                       const Buffer::Descriptor& desc)
    : device_{dev.device} {
    std::vector<uint32_t> indices;
    if (dev.queueIndices.HasSeperateQueue()) {
        indices.emplace_back(dev.queueIndices.graphicsIndex.value());
        indices.emplace_back(dev.queueIndices.presentIndex.value());
    } else {
        indices.emplace_back(dev.queueIndices.graphicsIndex.value());
    }

    size_ = desc.size;
    createBuffer(desc.size, BufferUsage2Vk(desc.usage), indices,
                 desc.mappedAtCreation);
    allocateMem(phyDev, getMemoryProperty(phyDev, desc));
    VK_CALL_NO_VALUE(dev.device.bindBufferMemory(buffer, mem, 0));

    if (desc.mappedAtCreation) {
        MapAsync(Flags<Buffer::Mode>(Buffer::Mode::Read) | Buffer::Mode::Write,
                        0, Size());
    }
}

void BufferImpl::createBuffer(uint64_t size, vk::BufferUsageFlags usage,
                              const std::vector<uint32_t>& indices,
                              bool hostVisible) {
    vk::BufferCreateInfo createInfo;
    createInfo.setSize(size).setUsage(usage);
    if (indices.size() > 1) {
        createInfo.setQueueFamilyIndices(indices);
        createInfo.setSharingMode(vk::SharingMode::eConcurrent);
    } else {
        createInfo.setSharingMode(vk::SharingMode::eExclusive);
    }

    VK_CALL(buffer, device_.createBuffer(createInfo));
}

void BufferImpl::allocateMem(vk::PhysicalDevice phyDevice,
                             vk::MemoryPropertyFlags flags) {
    auto requirements = device_.getBufferMemoryRequirements(buffer);
    auto type = FindMemoryType(phyDevice, requirements, flags);

    if (!type) {
        LOGE(log_tag::Vulkan, "find corresponding memory type failed");
    } else {
        vk::MemoryAllocateInfo allocInfo;
        allocInfo.setAllocationSize(requirements.size)
            .setMemoryTypeIndex(type.value());
        VK_CALL(mem, device_.allocateMemory(allocInfo));
    }
}

vk::MemoryPropertyFlags BufferImpl::getMemoryProperty(
    vk::PhysicalDevice phyDevice, const Buffer::Descriptor& desc) {
    uint32_t prop = 0;
    auto usageBits = desc.usage;
    if (usageBits & BufferUsage::MapRead || usageBits & BufferUsage::MapWrite ||
        desc.mappedAtCreation) {
        if (phyDevice.getProperties().limits.nonCoherentAtomSize == 0) {
            prop |=
                static_cast<uint32_t>(vk::MemoryPropertyFlagBits::eHostVisible);
            isMappingCoherence_ = false;
        } else {
            prop |= static_cast<uint32_t>(
                vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent);
            isMappingCoherence_ = true;
        }
    } else {
        prop |= static_cast<uint32_t>(vk::MemoryPropertyFlagBits::eDeviceLocal);
    }
    return static_cast<vk::MemoryPropertyFlags>(prop);
}

BufferImpl::~BufferImpl() {
    if (mapState_ != Buffer::MapState::Unmapped) {
        Unmap();
    }
    device_.freeMemory(mem);
    device_.destroyBuffer(buffer);
    mem = nullptr;
    buffer = nullptr;
    device_ = nullptr;
}

enum Buffer::MapState BufferImpl::MapState() const {
    return mapState_;
}

uint64_t BufferImpl::Size() const {
    return size_;
}

void BufferImpl::Unmap() {
    device_.unmapMemory(mem);
    mapState_ = Buffer::MapState::Unmapped;
    map_ = nullptr;
}

void BufferImpl::MapAsync(Flags<Buffer::Mode> mode, uint64_t offset,
                          uint64_t size) {
    VK_CALL(map_, device_.mapMemory(mem, offset, size));
    if (map_) {
        mappedOffset_ = offset;
        mappedSize_ = size;
        mapState_ = Buffer::MapState::Mapped;
    }
}

void* BufferImpl::GetMappedRange() {
    return GetMappedRange(0, size_);
}

void* BufferImpl::GetMappedRange(uint64_t offset) {
    return GetMappedRange(offset, size_ - offset);
}

void* BufferImpl::GetMappedRange(uint64_t offset, uint64_t size) {
    return (char*)map_ + offset;
}

void BufferImpl::Flush() {
    vk::MappedMemoryRange range;
    range.setMemory(mem).setOffset(mappedOffset_).setSize(mappedSize_);
    VK_CALL_NO_VALUE(device_.flushMappedMemoryRanges(range));
}

void BufferImpl::Flush(uint64_t offset, uint64_t size) {
    vk::MappedMemoryRange range;
    range.setMemory(mem).setOffset(offset).setSize(size);
    VK_CALL_NO_VALUE(device_.flushMappedMemoryRanges(range));
}

bool BufferImpl::IsMappingCoherence() const {
    return isMappingCoherence_;
}

}  // namespace nickel::rhi::vulkan