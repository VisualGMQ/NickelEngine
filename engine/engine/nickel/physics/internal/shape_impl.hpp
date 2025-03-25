#pragma once
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/common/transform.hpp"
#include "nickel/physics/collision_group.hpp"
#include "nickel/physics/geometry.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/material.hpp"
#include <span>

namespace nickel::physics {

class ContextImpl;
class ShapeImpl;
class ShapeConstImpl;

class ShapeImpl: public RefCountable {
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

    void SetCollisionGroup(CollisionGroup);
    CollisionGroup GetCollisionGroup() const;
    void EnableGenerateHitEvent(bool enable);
    bool IsEnabledGenerateHitEvent() const;
    void EnableSceneQuery(bool enable);
    bool IsEnableSceneQuery() const;
    void EnableSimulate(bool enable);
    bool IsEnableSimulate() const;
    void SetTrigger(bool enable);
    bool IsTrigger() const;

    void SetSimulateBehaviorOverlap(CollisionGroup, bool);
    bool IsSimulateBehaviorOverlap(CollisionGroup) const;
    void SetSimulateBehaviorBlock(CollisionGroup, bool);
    bool IsSimulateBehaviorBlock(CollisionGroup) const;
    void SetSimulateBehaviorNoCollide(CollisionGroup);
    bool IsSimulateBehaviorNoCollide(CollisionGroup) const;

    physx::PxShape* m_shape{};

protected:
    ContextImpl* m_ctx{};
};

class ShapeConstImpl : protected ShapeImpl {
public:
    ShapeConstImpl() = default;
    ShapeConstImpl(ContextImpl* ctx, const physx::PxShape* shape);

    using ShapeImpl::GetLocalPose;
    using ShapeImpl::IncRefcount;
    void DecRefcount() override;
};

}  // namespace nickel::physics