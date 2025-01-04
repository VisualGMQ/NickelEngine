#include "nickel/graphics/bind_group.hpp"
#include "nickel/graphics/internal/bind_group_impl.hpp"

namespace nickel::graphics {

BindGroup::BindGroup(BindGroupImpl* impl) : m_impl{impl} {}

BindGroup::BindGroup(const BindGroup& o) : m_impl{o.m_impl} {
    m_impl->IncRefcount();
}

BindGroup::BindGroup(BindGroup&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

BindGroup& BindGroup::operator=(const BindGroup& o) noexcept {
    if (&o != this) {
        m_impl->DecRefcount();
        m_impl = o.m_impl;
        m_impl->IncRefcount();
    }
    return *this;
}

BindGroup& BindGroup::operator=(BindGroup&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

BindGroup::~BindGroup() {
    m_impl->DecRefcount();
}

const BindGroupImpl& BindGroup::Impl() const noexcept {
    return *m_impl;
}

BindGroupImpl& BindGroup::Impl() noexcept {
    return *m_impl;
}

}  // namespace nickel::graphics