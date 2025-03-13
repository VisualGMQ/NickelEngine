#include "nickel/physics/internal/material_impl.hpp"

#include "nickel/physics/internal/context_impl.hpp"
#include "nickel/physics/internal/enum_convert.hpp"

namespace nickel::physics {

MaterialImpl::MaterialImpl(ContextImpl* ctx, physx::PxMaterial* mtl)
    : m_ctx{ctx}, m_mtl{mtl} {
    if (m_mtl) {
        m_mtl->acquireReference();
    }
}

MaterialImpl::~MaterialImpl() {
    if (m_mtl) {
        m_mtl->release();
    }
}

void MaterialImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_ctx->m_material_allocator.MarkAsGarbage(this);
    }
}

void MaterialImpl::SetDynamicFriction(float friction) {
    m_mtl->setDynamicFriction(friction);
}

void MaterialImpl::SetStaticFriction(float friction) {
    m_mtl->setStaticFriction(friction);
}

void MaterialImpl::SetRestitution(float restitution) {
    m_mtl->setRestitution(restitution);
}

void MaterialImpl::SetDamping(float damping) {
    m_mtl->setDamping(damping);
}

float MaterialImpl::GetDamping() const {
    return m_mtl->getDamping();
}

float MaterialImpl::GetStaticFriction() const {
    return m_mtl->getStaticFriction();
}

float MaterialImpl::GetDynamicFriction() const {
    return m_mtl->getDynamicFriction();
}

float MaterialImpl::GetRestitution() const {
    return m_mtl->getRestitution();
}

void MaterialImpl::setFrictionCombineMode(CombineMode mode) {
    m_mtl->setFrictionCombineMode(CombineMode2PhysX(mode));
}

void MaterialImpl::setRestitutionCombineMode(CombineMode mode) {
    m_mtl->setRestitutionCombineMode(CombineMode2PhysX(mode));
}

void MaterialImpl::setDampingCombineMode(CombineMode mode) {
    m_mtl->setDampingCombineMode(CombineMode2PhysX(mode));
}

}  // namespace nickel::physics