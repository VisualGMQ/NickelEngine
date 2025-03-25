#pragma once
#include "nickel/common/transform.hpp"
#include "nickel/physics/collision_group.hpp"
#include "nickel/physics/filter.hpp"
#include "nickel/physics/geometry.hpp"
#include "nickel/physics/material.hpp"

namespace nickel::physics {
class ShapeImpl;
class ShapeConstImpl;
class ContextImpl;

class Shape {
public:
    Shape() = default;
    Shape(ShapeImpl* impl);
    Shape(const Shape& o);
    Shape(Shape&& o) noexcept;
    Shape& operator=(const Shape& other);
    Shape& operator=(Shape&& other) noexcept;
    ~Shape();

    const ShapeImpl* GetImpl() const;
    ShapeImpl* GetImpl();

    void SetMaterials(std::span<Material> materials);
    void SetMaterial(Material& materials);

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

    void SetGeometry(const Geometry&);

private:
    ShapeImpl* m_impl{};
};

class ShapeConst {
public:
    ShapeConst() = default;
    ShapeConst(ShapeConstImpl* impl);
    ShapeConst(const ShapeConst& o);
    ShapeConst(ShapeConst&& o) noexcept;
    ShapeConst& operator=(const ShapeConst& other);
    ShapeConst& operator=(ShapeConst&& other) noexcept;
    ~ShapeConst();

    const ShapeConstImpl* GetImpl() const;
    ShapeConstImpl* GetImpl();

    Transform GetLocalPose() const;

private:
    ShapeConstImpl* m_impl{};
};

}  // namespace nickel::physics