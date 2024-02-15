#pragma once

#include "graphics/rhi/impl/buffer.hpp"

namespace nickel::rhi::null {

class BufferImpl : public rhi::BufferImpl {
public:
    enum Buffer::MapState MapState() const;
    uint64_t Size() const;
    void Unmap();
    void MapAsync(Flags<Buffer::Mode>, uint64_t offset, uint64_t size);
    void* GetMappedRange();
    void* GetMappedRange(uint64_t offset);
    void* GetMappedRange(uint64_t offset, uint64_t size);
};

}  // namespace nickel::rhi::null