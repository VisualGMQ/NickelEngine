#include "nickel/physics/material.hpp"

#include "nickel/physics/internal/material_impl.hpp"

namespace nickel::physics {

Material::Material(MaterialImpl* impl) : m_impl{impl} {}

Material::Material(const Material& o) : m_impl{o.m_impl} {
    if (o.m_impl) {
        o.m_impl->IncRefcount();
    }
}

Material::Material(Material&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

Material& Material::operator=(const Material& o) {
    if (&o != this) {
        if (m_impl) {
            m_impl->DecRefcount();
        }
        m_impl = o.m_impl;
        if (o.m_impl) {
            o.m_impl->IncRefcount();
        }
    }
    return *this;
}

Material& Material::operator=(Material&& o) noexcept {
    if (&o != this) {
        if (m_impl) {
            m_impl->DecRefcount();
        }
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

Material::~Material() {
    if (m_impl) {
        m_impl->DecRefcount();
    }
}

void Material::SetDynamicFriction(float friction) {
    m_impl->SetDynamicFriction(friction);
}

void Material::SetStaticFriction(float friction) {
    m_impl->SetStaticFriction(friction);
}

void Material::SetRestitution(float restitution) {
    m_impl->SetRestitution(restitution);
}

void Material::SetDamping(float damping) {
    m_impl->SetDamping(damping);
}

float Material::GetDamping() const {
    return m_impl->GetDamping();
}

float Material::GetStaticFriction() const {
    return m_impl->GetStaticFriction();
}

float Material::GetDynamicFriction() const {
    return m_impl->GetDynamicFriction();
}

float Material::GetRestitution() const {
    return m_impl->GetRestitution();
}

void Material::SetFrictionCombineMode(CombineMode mode) {
    m_impl->setFrictionCombineMode(mode);
}

void Material::SetRestitutionCombineMode(CombineMode mode) {
    m_impl->setRestitutionCombineMode(mode);
}

void Material::SetDampingCombineMode(CombineMode mode) {
    m_impl->setDampingCombineMode(mode);
}

MaterialImpl* Material::GetImpl() {
    return m_impl;
}

const MaterialImpl* Material::GetImpl() const {
    return m_impl;
}

Material::operator bool() const noexcept {
    return m_impl;
}

}  // namespace nickel::physics