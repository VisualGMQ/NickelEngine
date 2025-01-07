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
    if (m_impl) {
        m_impl->DecRefcount();
    }
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

void Buffer::Release() {
    if (m_impl) {
        m_impl->DecRefcount();
        m_impl = nullptr;
    }
}


}  // namespace nickel::graphics