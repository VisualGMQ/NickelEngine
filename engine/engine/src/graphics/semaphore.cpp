﻿#include "nickel/graphics/semaphore.hpp"

#include "nickel/graphics/internal/semaphore_impl.hpp"

namespace nickel::graphics {

Semaphore::Semaphore(SemaphoreImpl* impl) : m_impl{impl} {}

Semaphore::Semaphore(const Semaphore& o) : m_impl{o.m_impl} {
    m_impl->IncRefcount();
}

Semaphore::Semaphore(Semaphore&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

Semaphore& Semaphore::operator=(const Semaphore& o) noexcept {
    if (&o != this) {
        m_impl->DecRefcount();
        m_impl = o.m_impl;
        m_impl->IncRefcount();
    }
    return *this;
}

Semaphore& Semaphore::operator=(Semaphore&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

Semaphore::~Semaphore() {
    m_impl->DecRefcount();
}

const SemaphoreImpl& Semaphore::Impl() const noexcept {
    return *m_impl;
}

SemaphoreImpl& Semaphore::Impl() noexcept {
    return *m_impl;
}

}  // namespace nickel::graphics