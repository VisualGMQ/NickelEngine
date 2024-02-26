#pragma once

#include "graphics/rhi/impl/buffer.hpp"

namespace nickel::rhi::null {

class BufferImpl : public rhi::BufferImpl {
public:
    enum Buffer::MapState MapState() const override;
    uint64_t Size() const override;
    void Unmap() override;
    void MapAsync(Flags<Buffer::Mode>, uint64_t offset, uint64_t size) override;
    void* GetMappedRange() override;
    void* GetMappedRange(uint64_t offset) override;
    void* GetMappedRange(uint64_t offset, uint64_t size) override;
    void Flush() override;
    void Flush(uint64_t, uint64_t) override;
    bool IsMappingCoherence() const override;
};

}  // namespace nickel::rhi::null