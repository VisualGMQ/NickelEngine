#pragma once
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/common/transform.hpp"
#include "nickel/physics/geometry.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/material.hpp"
#include <span>

namespace nickel::physics {

class ContextImpl;
class ShapeImpl;
class ShapeImplConst;

class ShapeImpl : public RefCountable {
public:
    ShapeImpl() = default;
    ShapeImpl(ContextImpl* ctx, physx::PxShape* shape);
    virtual ~ShapeImpl() = default;

    void DecRefcount() override;

    void SetMaterials(std::span<Material> materials);
    void SetMaterial(Material& materials);

    void SetGeometry(const Geometry&);

    void SetLocalPose(const Vec3&, const Quat&);
    Transform GetLocalPose() const;

    physx::PxShape* m_shape{};

protected:
    ContextImpl* m_ctx{};
};

class ShapeImplConst : protected ShapeImpl {
public:
    ShapeImplConst() = default;
    ShapeImplConst(ContextImpl* ctx, const physx::PxShape* shape);

    using ShapeImpl::GetLocalPose;
    using ShapeImpl::IncRefcount;
    void DecRefcount() override;
};

}  // namespace nickel::physics