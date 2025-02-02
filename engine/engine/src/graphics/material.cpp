#include "nickel/graphics/material.hpp"
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/graphics/internal/material3d_impl.hpp"

namespace nickel::graphics {

Material3D::Material3D(Material3DImpl* impl)
    : m_impl{impl} {
}

Material3D::Material3D(const Material3D& o) noexcept
    : m_impl{o.m_impl} {
    m_impl->IncRefcount();
}

Material3D::Material3D(Material3D&& o) noexcept
    : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

Material3D& Material3D::operator=(Material3D&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
    }
    return *this;
}

Material3D& Material3D::operator=(const Material3D& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        if (m_impl) {
            m_impl->IncRefcount();
        }
    }
    return *this;
}

Material3D::~Material3D() {
    if (m_impl) {
        m_impl->DecRefcount();
    }
}

Material3DImpl* Material3D::GetImpl() {
    return m_impl;
}

const Material3DImpl* Material3D::GetImpl() const {
    return m_impl;
}
}  // namespace nickel::graphics
