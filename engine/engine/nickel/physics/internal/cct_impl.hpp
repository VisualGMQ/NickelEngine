#pragma once
#include "nickel/common/flags.hpp"
#include "nickel/physics/cct.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/internal/scene_impl.hpp"
#include "nickel/physics/internal/context_impl.hpp"

namespace nickel::physics {
class SceneImpl;

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
                                         float elapsed_time);
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

}  // namespace nickel::physics