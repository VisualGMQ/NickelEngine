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

}  // namespace nickel::rhi::null