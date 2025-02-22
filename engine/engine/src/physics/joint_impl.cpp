#include "nickel/physics/internal/joint_impl.hpp"

#include "nickel/physics/internal/context_impl.hpp"
#include "nickel/physics/internal/enum_convert.hpp"
#include "nickel/physics/internal/rigidbody_impl.hpp"
#include "nickel/physics/internal/util.hpp"

namespace nickel::physics {
D6JointImpl::D6JointImpl(ContextImpl* ctx, physx::PxD6Joint* joint)
    : m_joint{joint}, m_ctx{ctx} {}

D6JointImpl::~D6JointImpl() {
    if (m_joint) {
        m_joint->release();
    }
}

void D6JointImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_ctx->m_joint_allocator.MarkAsGarbage(this);
    }
}

void D6JointImpl::SetXMotion(D6Joint::Motion motion) {
    m_joint->setMotion(physx::PxD6Axis::eX, D6Motion2PhysX(motion));
}

void D6JointImpl::SetYMotion(D6Joint::Motion motion) {
    m_joint->setMotion(physx::PxD6Axis::eY, D6Motion2PhysX(motion));
}

void D6JointImpl::SetZMotion(D6Joint::Motion motion) {
    m_joint->setMotion(physx::PxD6Axis::eZ, D6Motion2PhysX(motion));
}

void D6JointImpl::SetSwing1Motion(D6Joint::Motion motion) {
    m_joint->setMotion(physx::PxD6Axis::eSWING1, D6Motion2PhysX(motion));
}

void D6JointImpl::SetSwing2Motion(D6Joint::Motion motion) {
    m_joint->setMotion(physx::PxD6Axis::eSWING2, D6Motion2PhysX(motion));
}

void D6JointImpl::SetTwistMotion(D6Joint::Motion motion) {
    m_joint->setMotion(physx::PxD6Axis::eTWIST, D6Motion2PhysX(motion));
}

D6Joint::Motion D6JointImpl::GetXMotion() const {
    return D6MotionFromPhysX(m_joint->getMotion(physx::PxD6Axis::eX));
}

D6Joint::Motion D6JointImpl::GetYMotion() const {
    return D6MotionFromPhysX(m_joint->getMotion(physx::PxD6Axis::eY));
}

D6Joint::Motion D6JointImpl::GetZMotion() const {
    return D6MotionFromPhysX(m_joint->getMotion(physx::PxD6Axis::eZ));
}

D6Joint::Motion D6JointImpl::GetSwing1Motion() const {
    return D6MotionFromPhysX(m_joint->getMotion(physx::PxD6Axis::eSWING1));
}

D6Joint::Motion D6JointImpl::GetSwing2Motion() const {
    return D6MotionFromPhysX(m_joint->getMotion(physx::PxD6Axis::eSWING2));
}

D6Joint::Motion D6JointImpl::GetTwistMotion() const {
    return D6MotionFromPhysX(m_joint->getMotion(physx::PxD6Axis::eTWIST));
}

void D6JointImpl::SetDistanceLimit(const D6Joint::LinearLimit& limit) {
    physx::PxJointLinearLimit l{limit.m_extent};
    l.damping = limit.m_damping;
    l.restitution = limit.m_restitution;
    l.bounceThreshold = limit.m_bounce_threshold;
    l.stiffness = limit.m_stiffness;
    m_joint->setDistanceLimit(l);
}

void D6JointImpl::SetTwistLimit(const D6Joint::AngularLimit& limit) {
    physx::PxJointAngularLimitPair l{limit.m_lower, limit.m_upper};
    l.damping = limit.m_damping;
    l.restitution = limit.m_restitution;
    l.bounceThreshold = limit.m_bounce_threshold;
    l.stiffness = limit.m_stiffness;
    m_joint->setTwistLimit(l);
}

void D6JointImpl::SetSwingLimit(const D6Joint::LimitCone& limit) {
    physx::PxJointLimitCone l{limit.m_y_angle, limit.m_z_angle};
    l.damping = limit.m_damping;
    l.restitution = limit.m_restitution;
    l.bounceThreshold = limit.m_bounce_threshold;
    l.stiffness = limit.m_stiffness;
    m_joint->setSwingLimit(l);
}

void D6JointImpl::SetPyramidLimit(const D6Joint::PyramidLimit& limit) {
    physx::PxJointLimitPyramid l{limit.m_y_limit_min, limit.m_y_limit_max,
                                 limit.m_z_limit_min, limit.m_z_limit_max};
    l.damping = limit.m_damping;
    l.restitution = limit.m_restitution;
    l.bounceThreshold = limit.m_bounce_threshold;
    l.stiffness = limit.m_stiffness;
    m_joint->setPyramidSwingLimit(l);
}

void D6JointImpl::SetActors(const RigidActor& actor1, const Vec3& p1,
                            const Quat& q1, const RigidActor& actor2,
                            const Vec3& p2, const Quat& q2) {
    SetActors(actor1, actor2);
    SetActor0LocalPose(p1, q1);
    SetActor1LocalPose(p2, q2);
}

void D6JointImpl::SetActors(const RigidActor& actor1,
                            const RigidActor& actor2) {
    m_joint->setActors(actor1.GetImpl()->m_actor, actor2.GetImpl()->m_actor);
}

D6Joint::LinearLimit D6JointImpl::GetDistanceLimit() const {
    auto limit = m_joint->getDistanceLimit();
    D6Joint::LinearLimit result;
    result.m_extent = limit.value;
    result.m_bounce_threshold = limit.bounceThreshold;
    result.m_stiffness = limit.stiffness;
    result.m_restitution = limit.restitution;
    result.m_damping = limit.damping;
    return result;
}

D6Joint::AngularLimit D6JointImpl::GetTwistLimit() const {
    auto limit = m_joint->getTwistLimit();
    D6Joint::AngularLimit result;
    result.m_lower = limit.lower;
    result.m_upper = limit.upper;
    result.m_bounce_threshold = limit.bounceThreshold;
    result.m_stiffness = limit.stiffness;
    result.m_restitution = limit.restitution;
    result.m_damping = limit.damping;
    return result;
}

D6Joint::LimitCone D6JointImpl::GetSwingLimit() const {
    auto limit = m_joint->getSwingLimit();
    D6Joint::LimitCone result;
    result.m_y_angle = limit.yAngle;
    result.m_z_angle = limit.zAngle;
    result.m_bounce_threshold = limit.bounceThreshold;
    result.m_stiffness = limit.stiffness;
    result.m_restitution = limit.restitution;
    result.m_damping = limit.damping;
    return result;
}

D6Joint::PyramidLimit D6JointImpl::GetPyramidLimit() const {
    auto limit = m_joint->getPyramidSwingLimit();
    D6Joint::PyramidLimit result;
    result.m_y_limit_min = limit.yAngleMin;
    result.m_y_limit_max = limit.yAngleMax;
    result.m_z_limit_min = limit.zAngleMin;
    result.m_z_limit_max = limit.zAngleMax;
    result.m_bounce_threshold = limit.bounceThreshold;
    result.m_stiffness = limit.stiffness;
    result.m_restitution = limit.restitution;
    result.m_damping = limit.damping;
    return result;
}

std::tuple<RigidActor, RigidActor> D6JointImpl::GetActors() {
    physx::PxRigidActor *r1, *r2;
    m_joint->getActors(r1, r2);
    return {RigidActor{m_ctx->m_rigid_actor_allocator.Allocate(m_ctx, r1)},
            RigidActor{m_ctx->m_rigid_actor_allocator.Allocate(m_ctx, r2)}};
}

void D6JointImpl::SetXDrive(const D6Joint::Drive& d) {
    m_joint->setDrive(physx::PxD6Drive::eX, D6JointDrive2PhysX(d));
}

void D6JointImpl::SetYDrive(const D6Joint::Drive& d) {
    m_joint->setDrive(physx::PxD6Drive::eY, D6JointDrive2PhysX(d));
}

void D6JointImpl::SetZDrive(const D6Joint::Drive& d) {
    m_joint->setDrive(physx::PxD6Drive::eZ, D6JointDrive2PhysX(d));
}

void D6JointImpl::SetSwingDrive(const D6Joint::Drive& d) {
    physx::PxD6JointDrive drive{d.m_stiffness, d.m_damping, d.m_force_limit,
                                d.m_is_acceleration};
    m_joint->setDrive(physx::PxD6Drive::eSWING, D6JointDrive2PhysX(d));
}

void D6JointImpl::SetTwistDrive(const D6Joint::Drive& d) {
    m_joint->setDrive(physx::PxD6Drive::eTWIST, D6JointDrive2PhysX(d));
}

void D6JointImpl::SetSlerpDrive(const D6Joint::Drive& d) {
    m_joint->setDrive(physx::PxD6Drive::eSLERP, D6JointDrive2PhysX(d));
}

void D6JointImpl::SetActor0LocalPose(const Vec3& p, const Quat& q) {
    m_joint->setLocalPose(physx::PxJointActorIndex::eACTOR0,
                          {Vec3ToPhysX(p), QuatToPhysX(q)});
}

void D6JointImpl::SetActor1LocalPose(const Vec3& p, const Quat& q) {
    m_joint->setLocalPose(physx::PxJointActorIndex::eACTOR1,
                          {Vec3ToPhysX(p), QuatToPhysX(q)});
}

D6Joint::Drive D6JointImpl::GetXDrive() const {
    return D6JointDriveFromPhysX(m_joint->getDrive(physx::PxD6Drive::eX));
}

D6Joint::Drive D6JointImpl::GetYDrive() const {
    return D6JointDriveFromPhysX(m_joint->getDrive(physx::PxD6Drive::eY));
}

D6Joint::Drive D6JointImpl::GetZDrive() const {
    return D6JointDriveFromPhysX(m_joint->getDrive(physx::PxD6Drive::eZ));
}

D6Joint::Drive D6JointImpl::GetSwingDrive() const {
    return D6JointDriveFromPhysX(m_joint->getDrive(physx::PxD6Drive::eSWING));
}

D6Joint::Drive D6JointImpl::GetTwistDrive() const {
    return D6JointDriveFromPhysX(m_joint->getDrive(physx::PxD6Drive::eTWIST));
}

D6Joint::Drive D6JointImpl::GetSlerpDrive() const {
    return D6JointDriveFromPhysX(m_joint->getDrive(physx::PxD6Drive::eSLERP));
}

void D6JointImpl::SetDrivePosition(const Vec3& p, const Quat& q) {
    m_joint->setDrivePosition({Vec3ToPhysX(p), QuatToPhysX(q)});
}

void D6JointImpl::SetDriveVelocity(const Vec3& linear, const Vec3& angular) {
    m_joint->setDriveVelocity(Vec3ToPhysX(linear), Vec3ToPhysX(angular));
}

Transform D6JointImpl::GetDrivePosition() const {
    return TransformFromPhysX(m_joint->getDrivePosition());
}

void D6JointImpl::GetDriveVelocity(Vec3& linear, Vec3& angular) const {
    physx::PxVec3 l, a;
    m_joint->getDriveVelocity(l, a);
    linear = Vec3FromPhysX(l);
    angular = Vec3FromPhysX(a);
}

void D6JointImpl::SetBreakForce(float force, float torque) {
    m_joint->setBreakForce(force, torque);
}

void D6JointImpl::GetBreakForce(float& force, float& torque) const {
    m_joint->getBreakForce(force, torque);
}

bool D6JointImpl::CanBeBroken(bool enable) const {
    return m_joint->getConstraintFlags() & physx::PxConstraintFlag::eBROKEN;
}

void D6JointImpl::EnableCollision(bool enable) {
    m_joint->setConstraintFlag(physx::PxConstraintFlag::eCOLLISION_ENABLED,
                               enable);
}

void D6JointImpl::EnableDriveLimitsAreForces(bool enable) {
    m_joint->setConstraintFlag(
        physx::PxConstraintFlag::eDRIVE_LIMITS_ARE_FORCES, enable);
}

void D6JointImpl::EnableExtendedLimits(bool enable) {
    m_joint->setConstraintFlag(physx::PxConstraintFlag::eENABLE_EXTENDED_LIMITS,
                               enable);
}

void D6JointImpl::DisableConstraint(bool disable) {
    m_joint->setConstraintFlag(physx::PxConstraintFlag::eDISABLE_CONSTRAINT,
                               disable);
}

void D6JointImpl::EnableAlwaysUpdate(bool enable) {
    m_joint->setConstraintFlag(physx::PxConstraintFlag::eALWAYS_UPDATE, enable);
}

bool D6JointImpl::IsEnableCollision() const {
    return m_joint->getConstraintFlags() &
           physx::PxConstraintFlag::eCOLLISION_ENABLED;
}

bool D6JointImpl::IsEnableDriveLimitsAreForces() const {
    return m_joint->getConstraintFlags() &
           physx::PxConstraintFlag::eDRIVE_LIMITS_ARE_FORCES;
}

bool D6JointImpl::IsEnableExtendedLimits() const {
    return m_joint->getConstraintFlags() &
           physx::PxConstraintFlag::eENABLE_EXTENDED_LIMITS;
}

bool D6JointImpl::IsDisableConstraint() const {
    return m_joint->getConstraintFlags() &
           physx::PxConstraintFlag::eDISABLE_CONSTRAINT;
}

bool D6JointImpl::IsEnableAlwaysUpdate() const {
    return m_joint->getConstraintFlags() &
           physx::PxConstraintFlag::eALWAYS_UPDATE;
}

}  // namespace nickel::physics