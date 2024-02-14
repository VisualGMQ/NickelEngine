#pragma once

#include "graphics/rhi/buffer.hpp"

namespace nickel::rhi {

class BufferImpl {
public:
    virtual ~BufferImpl() = default;

    virtual enum Buffer::MapState MapState() const = 0;
    virtual uint64_t Size() const = 0;
    virtual void Unmap() = 0;
    virtual void MapAsync(Flags<Buffer::Mode>, uint64_t offset, uint64_t size) = 0;
    virtual void* GetMappedRange() = 0;
    virtual void* GetMappedRange(uint64_t offset) = 0;
    virtual void* GetMappedRange(uint64_t offset, uint64_t size) = 0;
};

}