﻿#include "nickel/graphics/lowlevel/fence.hpp"

#include "nickel/graphics/lowlevel/internal/fence_impl.hpp"

namespace nickel::graphics {

Fence::Fence(FenceImpl* impl) : m_impl{impl} {}

Fence::Fence(const Fence& o) : m_impl{o.m_impl} {
    if (m_impl) {
        m_impl->IncRefcount();
    }
}

Fence::Fence(Fence&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

Fence& Fence::operator=(const Fence& o) noexcept {
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

Fence& Fence::operator=(Fence&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

Fence::~Fence() {
    Release();
}

Fence::operator bool() const noexcept {
    return m_impl;
}

const FenceImpl& Fence::Impl() const noexcept {
    return *m_impl;
}

FenceImpl& Fence::Impl() noexcept {
    return *m_impl;
}

void Fence::Release() {
    if (m_impl) {
        m_impl->DecRefcount();
        m_impl = nullptr;
    }
}

}  // namespace nickel::graphics