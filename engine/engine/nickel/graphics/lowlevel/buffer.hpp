#pragma once
#include "nickel/common/dllexport.hpp"
#include "nickel/common/flags.hpp"
#include "nickel/graphics/lowlevel/enums.hpp"

namespace nickel::graphics {

class BufferImpl;

class NICKEL_API Buffer {
public:
    struct Descriptor {
        uint64_t m_size;
        Flags<BufferUsage> m_usage;
        MemoryType m_memory_type;
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
    void MapAsync();
    [[nodiscard]] void* GetMappedRange();
    void* GetMappedRange(uint64_t offset);
    void Flush();
    void Flush(uint64_t offset, uint64_t size);
    void BuffData(void* data, size_t size, size_t offset);

    operator bool() const noexcept;

private:
    BufferImpl* m_impl{};
};

}  // namespace nickel::graphics