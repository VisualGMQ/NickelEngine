#pragma once

#include "nickel/graphics/internal/refcountable.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

class MemoryImpl : public RefCountable {
public:
    MemoryImpl(VkDevice, size_t size, uint32_t memory_type_index);
    ~MemoryImpl();
    size_t Size() const noexcept;

    VkDeviceMemory m_memory;

private:
    VkDevice m_device;
    size_t m_size;
};

}  // namespace nickel::graphics