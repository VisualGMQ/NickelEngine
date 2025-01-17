#include "nickel/graphics/buffer.hpp"
#include "nickel/graphics/internal/buffer_impl.hpp"

namespace nickel::graphics {

Buffer::Buffer(BufferImpl* impl) : m_impl{impl} {}

Buffer::Buffer(const Buffer& o) : m_impl{o.m_impl} {
    if (m_impl) {
        m_impl->IncRefcount();
    }
}

Buffer::Buffer(Buffer&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

Buffer& Buffer::operator=(const Buffer& o) noexcept {
    if (&o != this) {
        if (m_impl) {
            m_impl->DecRefcount();
        }
        m_impl = o.m_impl;
        if (m_impl) {
            m_impl->IncRefcount();
        }
    }
    return *this;
}

Buffer& Buffer::operator=(Buffer&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

Buffer::~Buffer() {
    Release();
}

Buffer::operator bool() const noexcept {
    return m_impl;
}

const BufferImpl& Buffer::Impl() const noexcept {
    return *m_impl;
}

BufferImpl& Buffer::Impl() noexcept {
    return *m_impl;
}

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

void Buffer::Release() {
    if (m_impl) {
        m_impl->DecRefcount();
        m_impl = nullptr;
    }
}


}  // namespace nickel::graphics