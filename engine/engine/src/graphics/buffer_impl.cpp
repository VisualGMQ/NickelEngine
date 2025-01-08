#include "nickel/graphics/internal/buffer_impl.hpp"
#include "nickel/graphics/buffer.hpp"
#include "nickel/graphics/internal/common.hpp"
#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/graphics/internal/enum_convert.hpp"
#include "nickel/graphics/internal/memory_impl.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

BufferImpl::BufferImpl(DeviceImpl& dev, VkPhysicalDevice phyDev,
                       const Buffer::Descriptor& desc)
    : m_device{dev.m_device} {
    std::vector<uint32_t> indices;
    if (dev.m_queue_indices.HasSeparateQueue()) {
        indices.emplace_back(dev.m_queue_indices.m_graphics_index.value());
        indices.emplace_back(dev.m_queue_indices.m_present_index.value());
    } else {
        indices.emplace_back(dev.m_queue_indices.m_graphics_index.value());
    }

    m_size = desc.m_size;
    createBuffer(dev, desc);
    allocateMem(
        dev, phyDev,
        static_cast<VkMemoryPropertyFlagBits>(getMemoryProperty(phyDev, desc)));

    VK_CALL(vkBindBufferMemory(dev.m_device, m_buffer, m_memory->m_memory, 0));
}

void BufferImpl::createBuffer(DeviceImpl& device,
                              const Buffer::Descriptor& desc) {
    std::vector<uint32_t> indices;
    if (device.m_queue_indices.m_graphics_index ==
        device.m_queue_indices.m_present_index) {
        indices.push_back(device.m_queue_indices.m_graphics_index.value());
    } else {
        indices.push_back(device.m_queue_indices.m_graphics_index.value());
        indices.push_back(device.m_queue_indices.m_present_index.value());
    }

    VkBufferCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    ci.size = desc.m_size;
    ci.usage = static_cast<VkBufferUsageFlagBits>(BufferUsage2Vk(desc.m_usage));
    ci.queueFamilyIndexCount = indices.size();
    ci.pQueueFamilyIndices = indices.data();
    if (indices.size() > 1) {
        ci.sharingMode = VK_SHARING_MODE_CONCURRENT;
    } else {
        ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    VK_CALL(vkCreateBuffer(device.m_device, &ci, nullptr, &m_buffer));
}

void BufferImpl::allocateMem(DeviceImpl& device, VkPhysicalDevice phyDevice,
                             VkMemoryPropertyFlags flags) {
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(device.m_device, m_buffer, &requirements);
    auto type = FindMemoryType(phyDevice, requirements, flags);

    if (!type) {
        LOGE("find corresponding memory type failed");
    } else {
        VkMemoryAllocateInfo allocInfo;
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = requirements.size;
        allocInfo.memoryTypeIndex = type.value();
        VK_CALL(vkAllocateMemory(device.m_device, &allocInfo, nullptr,
                                 &m_memory->m_memory));
    }
}

Flags<VkMemoryPropertyFlagBits> BufferImpl::getMemoryProperty(
    VkPhysicalDevice phyDevice, const Buffer::Descriptor& desc) {
    Flags<VkMemoryPropertyFlagBits> mem_props;
    auto usageBits = desc.m_usage;
    if (usageBits == VK_BUFFER_USAGE_TRANSFER_SRC_BIT) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(phyDevice, &props);
        if (props.limits.nonCoherentAtomSize == 0) {
            mem_props |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            m_is_mapping_coherence = false;
        } else {
            mem_props |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            mem_props |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            m_is_mapping_coherence = true;
        }
    } else {
        mem_props |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }
    return mem_props;
}

BufferImpl::~BufferImpl() {
    if (m_map_state != Buffer::MapState::Unmapped) {
        Unmap();
    }
    delete m_memory;
    vkDestroyBuffer(m_device, m_buffer, nullptr);
}

enum Buffer::MapState BufferImpl::MapState() const {
    return m_map_state;
}

uint64_t BufferImpl::Size() const {
    return m_size;
}

void BufferImpl::Unmap() {
    vkUnmapMemory(m_device, m_memory->m_memory);
    m_map_state = Buffer::MapState::Unmapped;
    m_map = nullptr;
}

void BufferImpl::MapAsync(uint64_t offset, uint64_t size) {
    VK_CALL(vkMapMemory(m_device, m_memory->m_memory, offset, size, 0, &m_map));
    if (m_map) {
        m_mapped_offset = offset;
        m_mapped_size = size;
        m_map_state = Buffer::MapState::Mapped;
    }
}

void* BufferImpl::GetMappedRange() {
    return GetMappedRange(0);
}

void* BufferImpl::GetMappedRange(uint64_t offset) {
    return (char*)m_map + offset;
}

void BufferImpl::Flush() {
    VkMappedMemoryRange range{};
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = m_memory->m_memory;
    range.offset = m_mapped_offset;
    range.size = m_mapped_size;
    VK_CALL(vkFlushMappedMemoryRanges(m_device, 1, &range));
}

void BufferImpl::Flush(uint64_t offset, uint64_t size) {
    VkMappedMemoryRange range{};
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = m_memory->m_memory;
    range.offset = offset;
    range.size = size;
    VK_CALL(vkFlushMappedMemoryRanges(m_device, 1, &range));
}

}  // namespace nickel::graphics