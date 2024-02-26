#include "graphics/rhi/null/buffer.hpp"

namespace nickel::rhi::null {

enum Buffer::MapState BufferImpl::MapState() const {
    return Buffer::MapState::Unmapped;
}

uint64_t BufferImpl::Size() const {
    return 0;
}

void BufferImpl::Unmap() {}

void BufferImpl::MapAsync(Flags<Buffer::Mode>, uint64_t offset, uint64_t size) {
}

void* BufferImpl::GetMappedRange() {
    return nullptr;
}

void* BufferImpl::GetMappedRange(uint64_t offset) {
    return nullptr;
}

void* BufferImpl::GetMappedRange(uint64_t offset, uint64_t size) {
    return nullptr;
}

void BufferImpl::Flush() {}

void BufferImpl::Flush(uint64_t, uint64_t) {}

bool BufferImpl::IsMappingCoherence() const {
    return false;
}

}  // namespace nickel::rhi::null