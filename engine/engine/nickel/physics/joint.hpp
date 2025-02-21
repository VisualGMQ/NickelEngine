#pragma once
#include "nickel/common/math/math.hpp"
#include "nickel/physics/rigidbody.hpp"

namespace nickel::physics {

class D6JointImpl;

class D6Joint {
public:
    enum class Motion {
        Free,
        Locked,
        Limited,
    };

    struct Limit {
        float m_restitution{};
        float m_bounce_threshold{};
        float m_stiffness{};
        float m_damping{};
    };

    struct LinearLimit : public Limit {
        float m_extent;
    };

    struct AngularLimit : public Limit {
        float m_lower = -PI * 0.5;
        float m_upper = PI * 0.5;
    };

    struct LimitCone : public Limit {
        float m_y_angle = PI * 0.5;
        float m_z_angle = PI * 0.5;
    };

    struct PyramidLimit : public Limit {
        float m_y_limit_min = -PI * 0.5;
        float m_y_limit_max = PI * 0.5;
        float m_z_limit_min = -PI * 0.5;
        float m_z_limit_max = PI * 0.5;
    };

    struct Drive {
        float m_force_limit{};
        float m_stiffness{};
        float m_damping{};
        bool m_is_acceleration = false;
    };

    D6Joint() = default;
    D6Joint(D6JointImpl*);
    D6Joint(const D6Joint&);
    D6Joint(D6Joint&&) noexcept;
    D6Joint& operator=(const D6Joint&);
    D6Joint& operator=(D6Joint&&) noexcept;
    ~D6Joint();

    void SetXMotion(Motion);
    void SetYMotion(Motion);
    void SetZMotion(Motion);
    void SetSwing1Motion(Motion);
    void SetSwing2Motion(Motion);
    void SetTwistMotion(Motion);

    Motion GetXMotion() const;
    Motion GetYMotion() const;
    Motion GetZMotion() const;
    Motion GetSwing1Motion() const;
    Motion GetSwing2Motion() const;
    Motion GetTwistMotion() const;

    void SetDistanceLimit(const LinearLimit&);
    void SetTwistLimit(const AngularLimit&);
    void SetSwingLimit(const LimitCone&);
    void SetPyramidLimit(const PyramidLimit&);
    void SetActors(const RigidActor& actor1, const Vec3& p1, const Quat& q1,
                   const RigidActor& actor2, const Vec3& p2, const Quat& q2);
    void SetActors(const RigidActor& actor1, const RigidActor& actor2);
    
    LinearLimit GetDistanceLimit() const;
    AngularLimit GetTwistLimit() const;
    LimitCone GetSwingLimit() const;
    PyramidLimit GetPyramidLimit() const;
    std::tuple<RigidActor, RigidActor> GetActors();
    
    void SetActor0LocalPose(const Vec3& p, const Quat& q);
    void SetActor1LocalPose(const Vec3& p, const Quat& q);

    void SetXDrive(const Drive&);
    void SetYDrive(const Drive&);
    void SetZDrive(const Drive&);
    void SetSwingDrive(const Drive&);
    void SetTwistDrive(const Drive&);
    void SetSlerpDrive(const Drive&);

    Drive GetXDrive() const;
    Drive GetYDrive() const;
    Drive GetZDrive() const;
    Drive GetSwingDrive() const;
    Drive GetTwistDrive() const;
    Drive GetSlerpDrive() const;

    void SetDrivePosition(const Vec3&, const Quat&);
    void SetDriveVelocity(const Vec3& linear, const Vec3& angular);
    Transform GetDrivePosition() const;
    void GetDriveVelocity(Vec3& linear, Vec3& angular) const;

    void SetBreakForce(float force, float torque);
    void GetBreakForce(float& force, float& torque) const;

    void EnableCollision(bool enable);
    void EnableDriveLimitsAreForces(bool enable);
    void EnableExtendedLimits(bool enable);
    void DisableConstraint(bool disable);
    void EnableAlwaysUpdate(bool enable);
    bool CanBeBroken(bool enable) const;
    bool IsEnableCollision() const;
    bool IsEnableDriveLimitsAreForces() const;
    bool IsEnableExtendedLimits() const;
    bool IsDisableConstraint() const;
    bool IsEnableAlwaysUpdate() const;

private:
    D6JointImpl* m_impl{};
};

}  // namespace nickel::physics