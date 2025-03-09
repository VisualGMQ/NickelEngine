#pragma once
#include "nickel/common/flags.hpp"
#include "nickel/physics/cct.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "scene_impl.hpp"

namespace nickel::physics {
class SceneImpl;

class PhysXControllerFilterCallback : public physx::PxControllerFilterCallback {
public:
    PhysXControllerFilterCallback(physx::PxControllerManager&, ContextImpl& ctx,
                               SceneImpl& scene);

    bool filter(const physx::PxController& a,
                const physx::PxController& b) override {
        if (a.getType() == physx::PxControllerShapeType::eCAPSULE &&
            b.getType() == physx::PxControllerShapeType::eCAPSULE) {
            const physx::PxCapsuleController& capsule1 =
                static_cast<const physx::PxCapsuleController&>(a);
            const physx::PxCapsuleController& capsule2 =
                static_cast<const physx::PxCapsuleController&>(b);
            auto cct1 = m_scene.m_const_capsule_controller_allocator.Allocate(
                m_mgr, m_ctx, m_scene, capsule1);
            auto cct2 = m_scene.m_const_capsule_controller_allocator.Allocate(
                m_mgr, m_ctx, m_scene, capsule2);
            return m_callback(CapsuleControllerConst{cct1}, CapsuleControllerConst{cct2});
        }
        // TODO: add Box Controller
        NICKEL_CANT_REACH();
        return true;
    }

    ControllerFilterCallback m_callback;

private:
    physx::PxControllerManager& m_mgr;
    ContextImpl& m_ctx;
    SceneImpl& m_scene;
};

inline physx::PxControllerNonWalkableMode::Enum NonWalkableMode2PhysX(
    CCTDesc::NonWalkableMode mode) {
    switch (mode) {
        case CCTDesc::NonWalkableMode::PreventClimbing:
            return physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING;
        case CCTDesc::NonWalkableMode::PreventClimbingAndForceSliding:
            return physx::PxControllerNonWalkableMode::
                ePREVENT_CLIMBING_AND_FORCE_SLIDING;
    }
    NICKEL_CANT_REACH();
    return {};
}

inline physx::PxCapsuleClimbingMode::Enum ClimbingMode2PhysX(
    CapsuleController::Descriptor::ClimbingMode mode) {
    switch (mode) {
        case CapsuleController::Descriptor::ClimbingMode::Easy:
            return physx::PxCapsuleClimbingMode::eEASY;
        case CapsuleController::Descriptor::ClimbingMode::Constrained:
            return physx::PxCapsuleClimbingMode::eCONSTRAINED;
    }
    NICKEL_CANT_REACH();
    return {};
}

inline physx::PxControllerCollisionFlags CCTCollisionFlag2PhysX(
    Flags<CCTCollisionFlag> flag) {
    physx::PxControllerCollisionFlags bits{};
    if (flag == CCTCollisionFlag::Down) {
        bits |= physx::PxControllerCollisionFlag::eCOLLISION_DOWN;
    }
    if (flag == CCTCollisionFlag::Up) {
        bits |= physx::PxControllerCollisionFlag::eCOLLISION_UP;
    }
    if (flag == CCTCollisionFlag::Sides) {
        bits |= physx::PxControllerCollisionFlag::eCOLLISION_SIDES;
    }
    return bits;
}

inline Flags<CCTCollisionFlag> CCTCollisionFlagFromPhysX(
    physx::PxControllerCollisionFlags flag) {
    Flags<CCTCollisionFlag> bits{};
    if (flag & physx::PxControllerCollisionFlag::eCOLLISION_DOWN) {
        bits |= CCTCollisionFlag::Down;
    }
    if (flag & physx::PxControllerCollisionFlag::eCOLLISION_SIDES) {
        bits |= CCTCollisionFlag::Sides;
    }
    if (flag & physx::PxControllerCollisionFlag::eCOLLISION_UP) {
        bits |= CCTCollisionFlag::Up;
    }
    return bits;
}

inline CCTDesc::NonWalkableMode NonWalkableModeFromPhysX(
    physx::PxControllerNonWalkableMode::Enum mode) {
    switch (mode) {
        case physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING:
            return CCTDesc::NonWalkableMode::PreventClimbing;
        case physx::PxControllerNonWalkableMode::
            ePREVENT_CLIMBING_AND_FORCE_SLIDING:
            return CCTDesc::NonWalkableMode::PreventClimbingAndForceSliding;
    }
    NICKEL_CANT_REACH();
    return {};
}

inline CapsuleController::Descriptor::ClimbingMode ClimbingModeFromPhysX(
    physx::PxCapsuleClimbingMode::Enum mode) {
    switch (mode) {
        case physx::PxCapsuleClimbingMode::eEASY:
            return CapsuleController::Descriptor::ClimbingMode::Easy;
        case physx::PxCapsuleClimbingMode::eCONSTRAINED:
            return CapsuleController::Descriptor::ClimbingMode::Constrained;
    }
    NICKEL_CANT_REACH();
    return {};
}

class CapsuleControllerImpl : public RefCountable {
public:
    CapsuleControllerImpl() = default;
    CapsuleControllerImpl(physx::PxControllerManager&, ContextImpl& ctx,
                          SceneImpl& scene,
                          const CapsuleController::Descriptor&);
    CapsuleControllerImpl(physx::PxControllerManager&, ContextImpl& ctx,
                          SceneImpl& scene,
                          physx::PxCapsuleController* controller);
    ~CapsuleControllerImpl();

    void SetRadius(float radius);
    float GetRadius() const;
    void SetHeight(float height);
    float GetHeight() const;
    void Resize(float height);
    Flags<CCTCollisionFlag> MoveAndSlide(const Vec3& disp, float min_dist,
                                         float elapsed_time,
                                         ControllerFilters*);
    void SetPosition(const Vec3&);
    void SetFootPosition(const Vec3&);
    Vec3 GetPosition() const;
    Vec3 GetFootPosition() const;
    void SetStepOffset(float);
    float GetStepOffset() const;
    void SetUpDirection(const Vec3&);
    Vec3 GetUpDirection() const;
    CCTDesc::NonWalkableMode GetNonWalkableMode() const;
    void SetNonWalkableMode(CCTDesc::NonWalkableMode);
    CapsuleController::Descriptor::ClimbingMode GetClimbingMode() const;
    void SetClimbingMode(CapsuleController::Descriptor::ClimbingMode);

    physx::PxCapsuleController* m_cct{};

    void DecRefcount() override;

protected:
    ContextImpl* m_ctx{};
    SceneImpl* m_scene{};
    CCTType m_type{CCTType::Capsule};
};

class CapsuleControllerConstImpl : protected CapsuleControllerImpl {
public:
    CapsuleControllerConstImpl() = default;
    CapsuleControllerConstImpl(physx::PxControllerManager&, ContextImpl& ctx,
                               SceneImpl& scene,
                               const CapsuleController::Descriptor&);
    CapsuleControllerConstImpl(physx::PxControllerManager&, ContextImpl& ctx,
                               SceneImpl& scene,
                               physx::PxCapsuleController* controller);

    using CapsuleControllerImpl::GetClimbingMode;
    using CapsuleControllerImpl::GetFootPosition;
    using CapsuleControllerImpl::GetHeight;
    using CapsuleControllerImpl::GetNonWalkableMode;
    using CapsuleControllerImpl::GetPosition;
    using CapsuleControllerImpl::GetRadius;
    using CapsuleControllerImpl::GetStepOffset;
    using CapsuleControllerImpl::GetUpDirection;

    using RefCountable::IncRefcount;
    void DecRefcount() override;
};

}  // namespace nickel::physics