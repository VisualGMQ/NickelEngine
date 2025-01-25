#pragma once
#include "nickel/graphics/buffer.hpp"
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

class DeviceImpl;
class MemoryImpl;

class BufferImpl : public RefCountable {
public:
    BufferImpl(DeviceImpl&, VkPhysicalDevice, const Buffer::Descriptor&);
    ~BufferImpl();
    BufferImpl(const BufferImpl&) = delete;
    BufferImpl(BufferImpl&&) = delete;
    BufferImpl& operator=(const BufferImpl&) = delete;
    BufferImpl& operator=(BufferImpl&&) = delete;

    enum Buffer::MapState MapState() const;
    uint64_t Size() const;
    void Unmap();
    void MapAsync(uint64_t offset, uint64_t size);
    void MapAsync();
    void* GetMappedRange();
    void* GetMappedRange(uint64_t offset);
    void Flush();
    void Flush(uint64_t offset, uint64_t size);

    void DecRefcount() override;

    void BuffData(void* data, size_t size, size_t offset);

    VkBuffer m_buffer = VK_NULL_HANDLE;
    MemoryImpl* m_memory{};

private:
    DeviceImpl& m_device;
    uint64_t m_size{};
    enum Buffer::MapState m_map_state = Buffer::MapState::Unmapped;
    void* m_map{};
    uint64_t m_mapped_offset{};
    uint64_t m_mapped_size{};
    bool m_is_mapping_coherence{};

    VkMemoryPropertyFlags getMemoryProperty(VkPhysicalDevice phyDevice,
                                            const Buffer::Descriptor&);
    void createBuffer(DeviceImpl&, const Buffer::Descriptor&);
    void allocateMem(DeviceImpl&, VkPhysicalDevice phyDevice,
                     VkMemoryPropertyFlags flags);
};

}  // namespace nickel::graphics