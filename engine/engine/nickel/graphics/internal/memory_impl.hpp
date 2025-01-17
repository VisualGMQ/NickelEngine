#pragma once

#include "nickel/graphics/internal/refcountable.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

class DeviceImpl;

class MemoryImpl {
public:
    MemoryImpl(DeviceImpl&, uint64_t size, uint32_t memory_type_index);
    MemoryImpl(const MemoryImpl&) = delete;
    MemoryImpl(MemoryImpl&&) = delete;
    MemoryImpl& operator=(const MemoryImpl&) = delete;
    MemoryImpl& operator=(MemoryImpl&&) = delete;

    ~MemoryImpl();
    size_t Size() const noexcept;

    VkDeviceMemory m_memory = VK_NULL_HANDLE;

private:
    DeviceImpl& m_device;
    size_t m_size;
};

}  // namespace nickel::graphics