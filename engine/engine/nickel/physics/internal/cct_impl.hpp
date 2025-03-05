#pragma once
#include "nickel/physics/cct.hpp"
#include "nickel/physics/internal/pch.hpp"

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
    CapsuleControllerImpl(physx::PxControllerManager&, SceneImpl& scene,
                          const CapsuleController::Descriptor&);
    ~CapsuleControllerImpl();

    void SetRadius(float radius);
    float GetRadius() const;
    void SetHeight(float height);
    float GetHeight() const;
    void Resize(float height);
    void MoveAndSlide(const Vec3& disp, float min_dist, float elapsed_time);
    void SetPosition(const Vec3&);
    void SetFootPosition(const Vec3&);
    Vec3 GetPosition() const;
    Vec3 GetFootPosition() const;
    void SetStepOffset(float) const;
    float GetStepOffset() const;
    void SetUpDirection(const Vec3&);
    Vec3 GetUpDirection() const;
    CCTDesc::NonWalkableMode GetNonWalkableMode() const;
    void SetNonWalkableMode(CCTDesc::NonWalkableMode);
    CapsuleController::Descriptor::ClimbingMode GetClimbingMode() const;
    void SetClimbingMode(CapsuleController::Descriptor::ClimbingMode) const;

    physx::PxCapsuleController* m_cct{};

    void DecRefcount() override;

private:
    SceneImpl* m_scene{};
    CCTType m_type{CCTType::Capsule};
};

}  // namespace nickel::physics