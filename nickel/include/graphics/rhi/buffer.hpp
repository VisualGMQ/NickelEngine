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
    void Destroy();
    enum MapState MapState() const;
    uint64_t Size() const;
    void Unmap();
    void MapAsync(Flags<Mode>, uint64_t offset, uint64_t size);
    void* GetMappedRange();
    void* GetMappedRange(uint64_t offset);
    void* GetMappedRange(uint64_t offset, uint64_t size);

    operator bool() const { return impl_; }

    auto Impl() const { return impl_; }
    auto Impl() { return impl_; }

private:
    BufferImpl* impl_{};
};

}