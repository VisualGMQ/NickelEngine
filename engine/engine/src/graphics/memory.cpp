#include "nickel/graphics/memory.hpp"
#include "nickel/graphics/internal/memory_impl.hpp"

nickel::graphics::Memory::Memory(MemoryImpl* impl): m_impl{impl} { }

nickel::graphics::Memory::Memory(const Memory& o) noexcept : m_impl{o.m_impl} {
    m_impl->IncRefcount();
}

nickel::graphics::Memory::Memory(Memory&& o) noexcept {
    m_impl = o.m_impl;
    o.m_impl = nullptr;
}

nickel::graphics::Memory& nickel::graphics::Memory::operator=(
    const Memory& o) noexcept {
    if (&o != this) {
        m_impl->DecRefcount();
        m_impl = o.m_impl;
        m_impl->IncRefcount();
    }
    return *this;
}

nickel::graphics::Memory& nickel::graphics::Memory::operator=(
    Memory&& o) noexcept {
    if (&o != this) {
        m_impl->DecRefcount();
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

size_t nickel::graphics::Memory::Size() const noexcept {
    return m_impl->Size();
}