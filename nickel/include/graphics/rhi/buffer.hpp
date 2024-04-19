#pragma once

#include "graphics/rhi/common.hpp"
#include <cstdint>

namespace nickel::rhi {

class BufferImpl;
class DeviceImpl;
class AdapterImpl;

class Buffer {
public:
    enum class MapState {
        Unmapped,
        Mapped,
        Pending,
    };

    struct Descriptor final {
        bool mappedAtCreation = false;
        uint64_t size = 0;
        Flags<BufferUsage> usage;
    };

    enum class Mode {
        Read = 0x01,
        Write = 0x02,
    };
    
    Buffer() = default;
    Buffer(AdapterImpl&, DeviceImpl&, const Buffer::Descriptor&);
    Buffer(Buffer&& o) { swap(o, *this); }
    Buffer(const Buffer& o) = default;
    Buffer& operator=(const Buffer& o) = default;
    Buffer& operator=(Buffer&& o) noexcept {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }
    void Destroy();
    enum MapState MapState() const;
    uint64_t Size() const;
    void Unmap();
    void MapAsync(Flags<Mode>, uint64_t offset, uint64_t size);
    void* GetMappedRange();
    void* GetMappedRange(uint64_t offset);
    void* GetMappedRange(uint64_t offset, uint64_t size);
    void Flush();
    void Flush(uint64_t offset, uint64_t size);
    bool IsMappingCoherence() const;

    operator bool() const { return impl_; }

    auto Impl() const { return impl_; }
    auto Impl() { return impl_; }

private:
    BufferImpl* impl_{};

    friend void swap(Buffer& o1, Buffer& o2) noexcept {
        using std::swap;

        swap(o1.impl_, o2.impl_);
    }
};

}