#pragma once
#include "nickel/common/flags.hpp"
#include "nickel/common/impl_wrapper.hpp"
#include "nickel/common/math/math.hpp"
#include "nickel/physics/material.hpp"

namespace nickel::physics {

class CCTManagerImpl;

class CapsuleControllerImpl;

enum class CCTType {
    Box,
    Capsule,
};

enum class CCTCollisionFlag
{
    Sides = 1 << 0,
    Up = 1 << 1,
    Down = 1 << 2,
};


struct CCTDesc {
    enum class NonWalkableMode {
        PreventClimbing,
        PreventClimbingAndForceSliding,
    };

    Vec3 m_position;
    Vec3 m_up_dir{0, 1, 0};
    float m_slope_limit = 0;
    float m_invisible_wall_height = 0;
    float m_max_jump_height = 0;
    float m_contact_offset = 0.1f;
    float m_step_offset = 0.5;
    float m_density = 10.0f;
    float m_scale_coeff = 0.8;
    float m_volume_growth = 1.05;
    NonWalkableMode m_nonwalkable_mode = NonWalkableMode::PreventClimbing;
    bool m_register_deleteion_listener = true;
    Material m_material;

    // PxUserControllerHitReport* reportCallback;
    // PxControllerBehaviorCallback* behaviorCallback;
};

class CapsuleController : public ImplWrapper<CapsuleControllerImpl> {
public:
    struct Descriptor : public CCTDesc {
        enum class ClimbingMode {
            Easy,
            Constrained,
        };

        float m_radius = 0;
        float m_height = 0;
        ClimbingMode m_climbing_mode = ClimbingMode::Easy;
    };

    using ImplWrapper::ImplWrapper;

    void SetRadius(float radius);
    float GetRadius() const;
    void SetHeight(float height);
    float GetHeight() const;
    void Resize(float height);
    Flags<CCTCollisionFlag> MoveAndSlide(const Vec3& disp, float min_dist, float elapsed_time);
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
    Descriptor::ClimbingMode GetClimbingMode() const;
    void SetClimbingMode(Descriptor::ClimbingMode) const;
};

}  // namespace nickel::physics