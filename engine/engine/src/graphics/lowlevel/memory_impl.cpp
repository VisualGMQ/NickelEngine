#include "nickel/graphics/lowlevel/internal/memory_impl.hpp"

#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"

namespace nickel::graphics {

MemoryImpl::MemoryImpl(DeviceImpl& device, uint64_t size, uint32_t memory_type_index)
    : m_device{device} {
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = size;
    allocInfo.memoryTypeIndex = memory_type_index;
    VK_CALL(vkAllocateMemory(device.m_device, &allocInfo, nullptr, &m_memory));

    if (m_memory) {
        m_size = size;
    }
}

MemoryImpl::~MemoryImpl() {
    vkFreeMemory(m_device.m_device, m_memory, nullptr);
}

size_t MemoryImpl::Size() const noexcept {
    return m_size;
}

}  // namespace nickel::graphics