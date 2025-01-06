#include "nickel/graphics/memory.hpp"
#include "nickel/graphics/internal/memory_impl.hpp"

namespace nickel::graphics {

Memory::Memory(MemoryImpl* impl) : m_impl{impl} {}

Memory::Memory(const Memory& o) noexcept : m_impl{o.m_impl} {
    if (m_impl) {
        m_impl->IncRefcount();
    }
}

Memory::Memory(Memory&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

Memory& Memory::operator=(const Memory& o) noexcept {
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

Memory& Memory::operator=(Memory&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

Memory::~Memory() {
    if (m_impl) {
        m_impl->DecRefcount();
    }
}

Memory::operator bool() const noexcept {
    return m_impl;
}

const MemoryImpl& Memory::Impl() const noexcept {
    return *m_impl;
}

MemoryImpl& Memory::Impl() noexcept {
    return *m_impl;
}

}  // namespace nickel::graphics
