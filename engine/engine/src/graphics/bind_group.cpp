#include "nickel/graphics/bind_group.hpp"
#include "nickel/graphics/internal/bind_group_impl.hpp"

namespace nickel::graphics {

BindGroup::BindGroup(BindGroupImpl* impl) : m_impl{impl} {}

BindGroup::BindGroup(BindGroup&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

BindGroup& BindGroup::operator=(BindGroup&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

BindGroup::~BindGroup() {
    if (m_impl) {
        m_impl->PendingDelete();
    }
}

BindGroup::operator bool() const noexcept {
    return m_impl;
}

const BindGroup::Descriptor& BindGroup::GetDescriptor() const {
    return m_impl->GetDescriptor();
}

const BindGroupImpl& BindGroup::Impl() const noexcept {
    return *m_impl;
}

BindGroupImpl& BindGroup::Impl() noexcept {
    return *m_impl;
}

}  // namespace nickel::graphics