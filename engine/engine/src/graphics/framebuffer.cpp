#include "nickel/graphics/framebuffer.hpp"
#include "nickel/graphics/internal/framebuffer_impl.hpp"

namespace nickel::graphics {

Framebuffer::Framebuffer(FramebufferImpl* impl) : m_impl{impl} {}

Framebuffer::Framebuffer(const Framebuffer& o) : m_impl{o.m_impl} {
    if (m_impl) {
        m_impl->IncRefcount();
    }
}

Framebuffer::Framebuffer(Framebuffer&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

Framebuffer& Framebuffer::operator=(const Framebuffer& o) noexcept {
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

Framebuffer& Framebuffer::operator=(Framebuffer&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

Framebuffer::~Framebuffer() {
    Release();
}

Framebuffer::operator bool() const noexcept {
    return m_impl;
}

const FramebufferImpl& Framebuffer::Impl() const noexcept {
    return *m_impl;
}

FramebufferImpl& Framebuffer::Impl() noexcept {
    return *m_impl;
}

void Framebuffer::Release() {
    if (m_impl) {
        m_impl->Release();
        m_impl->DecRefcount();
        if (m_impl->Refcount() == 0) {
            m_impl->PendingDelete();
        }
        m_impl = nullptr;
    }
}

}  // namespace nickel::graphics