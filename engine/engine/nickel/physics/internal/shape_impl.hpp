#pragma once
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/common/transform.hpp"
#include "nickel/physics/geometry.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/material.hpp"
#include <span>

namespace nickel::physics {

class ContextImpl;

class ShapeImpl: public RefCountable {
public:
    ShapeImpl(ContextImpl* ctx, physx::PxShape* shape);
    void SetMaterials(std::span<Material> materials);
    void SetMaterial(Material& materials);

    void SetGeometry(const Geometry&);

    void SetLocalPose(const Vec3&, const Quat&);
    Transform GetLocalPose() const;

    void DecRefcount() override;

    physx::PxShape* m_shape{};

private:
    ContextImpl* m_ctx{};
};

}