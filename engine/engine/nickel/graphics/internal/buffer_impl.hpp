#pragma once
#include "nickel/graphics/buffer.hpp"
#include "nickel/graphics/internal/refcountable.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

class DeviceImpl;
class MemoryImpl;

class BufferImpl : public RefCountable {
public:
    BufferImpl(DeviceImpl&, VkPhysicalDevice, const Buffer::Descriptor&);
    ~BufferImpl();

    enum Buffer::MapState MapState() const;
    uint64_t Size() const;
    void Unmap();
    void MapAsync(uint64_t offset, uint64_t size);
    void* GetMappedRange();
    void* GetMappedRange(uint64_t offset);
    void Flush();
    void Flush(uint64_t offset, uint64_t size);
    void PendingDelete();

    VkBuffer m_buffer;
    MemoryImpl* m_memory;

private:
    DeviceImpl& m_device;
    uint64_t m_size{};
    enum Buffer::MapState m_map_state = Buffer::MapState::Unmapped;
    void* m_map{};
    uint64_t m_mapped_offset{};
    uint64_t m_mapped_size{};
    bool m_is_mapping_coherence{};

    Flags<VkMemoryPropertyFlagBits> getMemoryProperty(
        VkPhysicalDevice phyDevice, const Buffer::Descriptor&);
    void createBuffer(DeviceImpl&, const Buffer::Descriptor&);
    void allocateMem(DeviceImpl&, VkPhysicalDevice phyDevice,
                     VkMemoryPropertyFlags flags);
};

}  // namespace nickel::graphics