#pragma once
#include "nickel/common/flags.hpp"

namespace nickel::graphics {

class BufferImpl;

class Buffer {
public:
    struct Descriptor {
        size_t m_size;
        Flags<VkBufferUsageFlagBits> m_usage;
    };

    enum class MapState {
        Unmapped,
        Mapped,
    };

    explicit Buffer(BufferImpl*);
    Buffer(const Buffer&);
    Buffer(Buffer&&) noexcept;
    Buffer& operator=(const Buffer&) noexcept;
    Buffer& operator=(Buffer&&) noexcept;
    ~Buffer();

    const BufferImpl& Impl() const noexcept;
    BufferImpl& Impl() noexcept;

    MapState MapState() const;
    uint64_t Size() const;
    void Unmap();
    void MapAsync(uint64_t offset, uint64_t size);
    void* GetMappedRange();
    void* GetMappedRange(uint64_t offset);
    void Flush();
    void Flush(uint64_t offset, uint64_t size);

private:
    BufferImpl* m_impl;
};

}  // namespace nickel::graphics