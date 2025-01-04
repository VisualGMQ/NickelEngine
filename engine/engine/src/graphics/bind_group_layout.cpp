#include "nickel/graphics/bind_group_layout.hpp"
#include "nickel/graphics/internal/bind_group_layout_impl.hpp"

namespace nickel::graphics {

BindGroupLayout::BindGroupLayout(BindGroupLayoutImpl* impl) : m_impl{impl} {}

BindGroupLayout::BindGroupLayout(const BindGroupLayout& o) : m_impl{o.m_impl} {
    m_impl->IncRefcount();
}

BindGroupLayout::BindGroupLayout(BindGroupLayout&& o) noexcept
    : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

BindGroupLayout& BindGroupLayout::operator=(const BindGroupLayout& o) noexcept {
    if (&o != this) {
        m_impl->DecRefcount();
        m_impl = o.m_impl;
        m_impl->IncRefcount();
    }
    return *this;
}

BindGroupLayout& BindGroupLayout::operator=(BindGroupLayout&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

BindGroupLayout::~BindGroupLayout() {
    m_impl->DecRefcount();
}

const BindGroupLayoutImpl& BindGroupLayout::Impl() const noexcept {
    return *m_impl;
}

BindGroupLayoutImpl& BindGroupLayout::Impl() noexcept {
    return *m_impl;
}

}  // namespace nickel::graphics