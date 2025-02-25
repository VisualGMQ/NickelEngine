#include "nickel/physics/material.hpp"

#include "nickel/physics/internal/material_impl.hpp"

namespace nickel::physics {

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

}  // namespace nickel::physics