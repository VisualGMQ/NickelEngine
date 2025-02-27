#pragma once

#include "nickel/physics/enums.hpp"
#include "nickel/physics/internal/pch.hpp"

namespace nickel::physics {

class ContextImpl;

class MaterialImpl {
public:
    MaterialImpl(ContextImpl* ctx, physx::PxMaterial* mtl);
    ~MaterialImpl();

    void IncRefcount();
    void DecRefcount();
    uint32_t Refcount() const;

    void SetDynamicFriction(float friction);
    void SetStaticFriction(float friction);
    void SetRestitution(float);
    void SetDamping(float);

    float GetDamping() const;
    float GetStaticFriction() const;
    float GetDynamicFriction() const;
    float GetRestitution() const;

    void setFrictionCombineMode(CombineMode);
    void setRestitutionCombineMode(CombineMode);
    void setDampingCombineMode(CombineMode);

    physx::PxMaterial* m_mtl{};

private:
    ContextImpl* m_ctx{};
};

}  // namespace nickel::physics