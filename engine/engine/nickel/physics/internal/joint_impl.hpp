#pragma once
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/joint.hpp"
#include "nickel/physics/rigidbody.hpp"

namespace nickel::physics {

inline physx::PxD6JointDrive D6JointDrive2PhysX(const D6Joint::Drive& drive) {
    physx::PxD6JointDrive d;
    d.forceLimit = drive.m_force_limit;
    d.damping = drive.m_damping;
    d.stiffness = drive.m_stiffness;
    d.flags = drive.m_is_acceleration
                  ? physx::PxD6JointDriveFlag::eACCELERATION
                  : physx::PxD6JointDriveFlag::eOUTPUT_FORCE;
    return d;
}

inline D6Joint::Drive D6JointDriveFromPhysX(
    const physx::PxD6JointDrive& drive) {
    D6Joint::Drive d;
    d.m_damping = drive.damping;
    d.m_force_limit = drive.forceLimit;
    d.m_stiffness = drive.stiffness;
    d.m_is_acceleration =
        drive.flags & physx::PxD6JointDriveFlag::eACCELERATION;
    return d;
}

class ContextImpl;

class D6JointImpl : public RefCountable {
public:
    D6JointImpl() = default;
    D6JointImpl(ContextImpl* ctx, physx::PxD6Joint*);
    ~D6JointImpl();

    void SetXMotion(D6Joint::Motion);
    void SetYMotion(D6Joint::Motion);
    void SetZMotion(D6Joint::Motion);
    void SetSwing1Motion(D6Joint::Motion);
    void SetSwing2Motion(D6Joint::Motion);
    void SetTwistMotion(D6Joint::Motion);

    D6Joint::Motion GetXMotion() const;
    D6Joint::Motion GetYMotion() const;
    D6Joint::Motion GetZMotion() const;
    D6Joint::Motion GetSwing1Motion() const;
    D6Joint::Motion GetSwing2Motion() const;
    D6Joint::Motion GetTwistMotion() const;

    void SetDistanceLimit(const D6Joint::LinearLimit&);
    void SetTwistLimit(const D6Joint::AngularLimit&);
    void SetSwingLimit(const D6Joint::LimitCone&);
    void SetPyramidLimit(const D6Joint::PyramidLimit&);
    void SetActors(const RigidActor& actor1, const Vec3& p1, const Quat& q1,
                   const RigidActor& actor2, const Vec3& p2, const Quat& q2);

    D6Joint::LinearLimit GetDistanceLimit() const;
    D6Joint::AngularLimit GetTwistLimit() const;
    D6Joint::LimitCone GetSwingLimit() const;
    D6Joint::PyramidLimit GetPyramidLimit() const;
    std::tuple<RigidActor, RigidActor> GetActors();

    void SetXDrive(const D6Joint::Drive&);
    void SetYDrive(const D6Joint::Drive&);
    void SetZDrive(const D6Joint::Drive&);
    void SetSwingDrive(const D6Joint::Drive&);
    void SetTwistDrive(const D6Joint::Drive&);
    void SetSlerpDrive(const D6Joint::Drive&);

    D6Joint::Drive GetXDrive() const;
    D6Joint::Drive GetYDrive() const;
    D6Joint::Drive GetZDrive() const;
    D6Joint::Drive GetSwingDrive() const;
    D6Joint::Drive GetTwistDrive() const;
    D6Joint::Drive GetSlerpDrive() const;

    void SetDrivePosition(const Vec3&, const Quat&);
    void SetDriveVelocity(const Vec3& linear, const Vec3& angular);
    Transform GetDrivePosition() const;
    void GetDriveVelocity(Vec3& linear, Vec3& angular) const;

    void SetBreakForce(float force, float torque);
    void GetBreakForce(float& force, float& torque) const;

    physx::PxD6Joint* m_joint{};

private:
    ContextImpl* m_ctx{};
};

}  // namespace nickel::physics