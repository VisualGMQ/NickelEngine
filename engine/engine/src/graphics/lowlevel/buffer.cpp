#include "nickel/graphics/lowlevel/buffer.hpp"
#include "nickel/graphics/lowlevel/internal/buffer_impl.hpp"

namespace nickel::graphics {

enum Buffer::MapState Buffer::MapState() const {
    return m_impl->MapState();
}

uint64_t Buffer::Size() const {
    return m_impl->Size();
}

void Buffer::Unmap() {
    return m_impl->Unmap();
}

void Buffer::MapAsync(uint64_t offset, uint64_t size) {
    return m_impl->MapAsync(offset, size);
}

void Buffer::MapAsync() {
    return m_impl->MapAsync();
}

void* Buffer::GetMappedRange() {
    return m_impl->GetMappedRange();
}

void* Buffer::GetMappedRange(uint64_t offset) {
    return m_impl->GetMappedRange(offset);
}

void Buffer::Flush() {
    return m_impl->Flush();
}

void Buffer::Flush(uint64_t offset, uint64_t size) {
    return m_impl->Flush(offset, size);
}

void Buffer::BuffData(void* data, size_t size, size_t offset) {
    return m_impl->BuffData(data, size, offset);
}

}  // namespace nickel::graphics