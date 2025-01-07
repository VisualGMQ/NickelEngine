#pragma once
#include "nickel/common/flags.hpp"
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class BufferImpl;

class NICKEL_API Buffer {
public:
    struct Descriptor {
        size_t m_size;
        Flags<VkBufferUsageFlagBits> m_usage;
    };

    enum class MapState {
        Unmapped,
        Mapped,
    };

    Buffer() = default;
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
    
    operator bool() const noexcept;
    void Release();

private:
    BufferImpl* m_impl{};
};

}  // namespace nickel::graphics