#pragma once
#include "filter.hpp"
#include "nickel/common/flags.hpp"
#include "nickel/common/impl_wrapper.hpp"
#include "nickel/common/math/math.hpp"
#include "nickel/physics/material.hpp"

namespace nickel::physics {

class CCTManagerImpl;

class CapsuleControllerImpl;
class CapsuleControllerConstImpl;

enum class CCTType {
    Box,
    Capsule,
};

enum class CCTCollisionFlag {
    Sides = 1 << 0,
    Up = 1 << 1,
    Down = 1 << 2,
};

class ControllerConst;

using ControllerFilterCallback =
    std::function<bool(const ControllerConst&, const ControllerConst&)>;

class ControllerFilters {
public:
    const FilterData* m_filter_data{};
    QueryFilterCallback* m_filter_callback{};
    Flags<QueryFlag> m_query_flags =
        Flags{QueryFlag::Static} | QueryFlag::Dynamic | QueryFlag::PreFilter;
    ControllerFilterCallback m_cct_callback;
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

class Controller {
public:
    explicit Controller(CCTType type);
    virtual ~Controller() = default;
    virtual Flags<CCTCollisionFlag> MoveAndSlide(const Vec3& disp,
                                                 float min_dist,
                                                 float elapsed_time,
                                                 ControllerFilters* filter = nullptr) = 0;
    virtual void SetPosition(const Vec3&) = 0;
    virtual Vec3 GetPosition() const = 0;
    virtual void SetFootPosition(const Vec3&) = 0;
    virtual Vec3 GetFootPosition() const = 0;
    virtual void SetStepOffset(float) const = 0;
    virtual float GetStepOffset() const = 0;
    virtual void SetUpDirection(const Vec3&) = 0;
    virtual Vec3 GetUpDirection() const = 0;
    virtual CCTDesc::NonWalkableMode GetNonWalkableMode() const = 0;
    virtual void SetNonWalkableMode(CCTDesc::NonWalkableMode) = 0;

private:
    CCTType m_type;
};

class ControllerConst : protected Controller {
public:
    explicit ControllerConst(CCTType type);
    using Controller::GetFootPosition;
    using Controller::GetNonWalkableMode;
    using Controller::GetPosition;
    using Controller::GetStepOffset;
    using Controller::GetUpDirection;
    using Controller::SetStepOffset;
};

class CapsuleController : public Controller,
                          public ImplWrapper<CapsuleControllerImpl> {
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

    CapsuleController(CapsuleControllerImpl* impl);
    CapsuleController();
    void SetRadius(float radius);
    float GetRadius() const;
    void SetHeight(float height);
    float GetHeight() const;
    void Resize(float height);
    Flags<CCTCollisionFlag> MoveAndSlide(const Vec3& disp, float min_dist,
                                         float elapsed_time, ControllerFilters*) override;
    void SetPosition(const Vec3&) override;
    Vec3 GetPosition() const override;
    void SetFootPosition(const Vec3&) override;
    Vec3 GetFootPosition() const override;
    void SetStepOffset(float) const override;
    float GetStepOffset() const override;
    void SetUpDirection(const Vec3&) override;
    Vec3 GetUpDirection() const override;
    CCTDesc::NonWalkableMode GetNonWalkableMode() const override;
    void SetNonWalkableMode(CCTDesc::NonWalkableMode) override;
    Descriptor::ClimbingMode GetClimbingMode() const;
    void SetClimbingMode(Descriptor::ClimbingMode);
};

class CapsuleControllerConst : public ControllerConst,
                               public ImplWrapper<CapsuleControllerConstImpl> {
public:
    CapsuleControllerConst();
    CapsuleControllerConst(CapsuleControllerConstImpl* impl);

    float GetRadius() const;
    float GetHeight() const;
    Vec3 GetPosition() const;
    Vec3 GetFootPosition() const;
    float GetStepOffset() const;
    Vec3 GetUpDirection() const;
    CCTDesc::NonWalkableMode GetNonWalkableMode() const;
    CapsuleController::Descriptor::ClimbingMode GetClimbingMode() const;
};

}  // namespace nickel::physics