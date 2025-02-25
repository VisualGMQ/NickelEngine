#include "nickel/physics/joint.hpp"

#include "nickel/physics/internal/joint_impl.hpp"

namespace nickel::physics {

void D6Joint::SetXMotion(Motion motion) {
    m_impl->SetXMotion(motion);
}

void D6Joint::SetYMotion(Motion motion) {
    m_impl->SetYMotion(motion);
}

void D6Joint::SetZMotion(Motion motion) {
    m_impl->SetZMotion(motion);
}

void D6Joint::SetSwing1Motion(Motion motion) {
    m_impl->SetSwing1Motion(motion);
}

void D6Joint::SetSwing2Motion(Motion motion) {
    m_impl->SetSwing2Motion(motion);
}

void D6Joint::SetTwistMotion(Motion motion) {
    m_impl->SetTwistMotion(motion);
}

D6Joint::Motion D6Joint::GetXMotion() const {
    return m_impl->GetXMotion();
}

D6Joint::Motion D6Joint::GetYMotion() const {
    return m_impl->GetYMotion();
}

D6Joint::Motion D6Joint::GetZMotion() const {
    return m_impl->GetZMotion();
}

D6Joint::Motion D6Joint::GetSwing1Motion() const {
    return m_impl->GetSwing1Motion();
}

D6Joint::Motion D6Joint::GetSwing2Motion() const {
    return m_impl->GetSwing2Motion();
}

D6Joint::Motion D6Joint::GetTwistMotion() const {
    return m_impl->GetTwistMotion();
}

void D6Joint::SetDistanceLimit(const LinearLimit& limit) {
    m_impl->SetDistanceLimit(limit);
}

void D6Joint::SetTwistLimit(const AngularLimit& limit) {
    m_impl->SetTwistLimit(limit);
}

void D6Joint::SetSwingLimit(const LimitCone& limit) {
    m_impl->SetSwingLimit(limit);
}

void D6Joint::SetPyramidLimit(const PyramidLimit& limit) {
    m_impl->SetPyramidLimit(limit);
}

void D6Joint::SetActors(const RigidActor& actor1, const Vec3& p1,
                        const Quat& q1, const RigidActor& actor2,
                        const Vec3& p2, const Quat& q2) {
    m_impl->SetActors(actor1, p1, q1, actor2, p2, q2);
}

void D6Joint::SetActors(const RigidActor& actor1, const RigidActor& actor2) {
    m_impl->SetActors(actor1, actor2);
}

D6Joint::LinearLimit D6Joint::GetDistanceLimit() const {
    return m_impl->GetDistanceLimit();
}

D6Joint::AngularLimit D6Joint::GetTwistLimit() const {
    return m_impl->GetTwistLimit();
}

D6Joint::LimitCone D6Joint::GetSwingLimit() const {
    return m_impl->GetSwingLimit();
}

D6Joint::PyramidLimit D6Joint::GetPyramidLimit() const {
    return m_impl->GetPyramidLimit();
}

std::tuple<RigidActor, RigidActor> D6Joint::GetActors() {
    return m_impl->GetActors();
}

void D6Joint::SetActor0LocalPose(const Vec3& p, const Quat& q) {
    m_impl->SetActor0LocalPose(p, q);
}

void D6Joint::SetActor1LocalPose(const Vec3& p, const Quat& q) {
    m_impl->SetActor1LocalPose(p, q);
}

void D6Joint::SetXDrive(const Drive& drive) {
    m_impl->SetXDrive(drive);
}

void D6Joint::SetYDrive(const Drive& drive) {
    m_impl->SetYDrive(drive);
}

void D6Joint::SetZDrive(const Drive& drive) {
    m_impl->SetZDrive(drive);
}

void D6Joint::SetSwingDrive(const Drive& drive) {
    m_impl->SetSwingDrive(drive);
}

void D6Joint::SetTwistDrive(const Drive& drive) {
    m_impl->SetTwistDrive(drive);
}

void D6Joint::SetSlerpDrive(const Drive& drive) {
    m_impl->SetSlerpDrive(drive);
}

D6Joint::Drive D6Joint::GetXDrive() const {
    return m_impl->GetXDrive();
}

D6Joint::Drive D6Joint::GetYDrive() const {
    return m_impl->GetYDrive();
}

D6Joint::Drive D6Joint::GetZDrive() const {
    return m_impl->GetZDrive();
}

D6Joint::Drive D6Joint::GetSwingDrive() const {
    return m_impl->GetSwingDrive();
}

D6Joint::Drive D6Joint::GetTwistDrive() const {
    return m_impl->GetTwistDrive();
}

D6Joint::Drive D6Joint::GetSlerpDrive() const {
    return m_impl->GetSlerpDrive();
}

void D6Joint::SetDrivePosition(const Vec3& p, const Quat& q) {
    m_impl->SetDrivePosition(p, q);
}

void D6Joint::SetDriveVelocity(const Vec3& linear, const Vec3& angular) {
    m_impl->SetDriveVelocity(linear, angular);
}

Transform D6Joint::GetDrivePosition() const {
    return m_impl->GetDrivePosition();
}

void D6Joint::GetDriveVelocity(Vec3& linear, Vec3& angular) const {
    m_impl->GetDriveVelocity(linear, angular);
}

void D6Joint::SetBreakForce(float force, float torque) {
    m_impl->SetBreakForce(force, torque);
}

void D6Joint::GetBreakForce(float& force, float& torque) const {
    m_impl->GetBreakForce(force, torque);
}

void D6Joint::EnableCollision(bool enable) {
    m_impl->EnableCollision(enable);
}

void D6Joint::EnableDriveLimitsAreForces(bool enable) {
    m_impl->EnableDriveLimitsAreForces(enable);
}

void D6Joint::EnableExtendedLimits(bool enable) {
    m_impl->EnableExtendedLimits(enable);
}

void D6Joint::DisableConstraint(bool disable) {
    m_impl->DisableConstraint(disable);
}

void D6Joint::EnableAlwaysUpdate(bool enable) {
    m_impl->EnableAlwaysUpdate(enable);
}

bool D6Joint::CanBeBroken(bool enable) const {
    return m_impl->CanBeBroken(enable);
}

bool D6Joint::IsEnableCollision() const {
    return m_impl->IsEnableCollision();
}

bool D6Joint::IsEnableDriveLimitsAreForces() const {
    return m_impl->IsEnableDriveLimitsAreForces();
}

bool D6Joint::IsEnableExtendedLimits() const {
    return m_impl->IsEnableExtendedLimits();
}

bool D6Joint::IsDisableConstraint() const {
    return m_impl->IsDisableConstraint();
}

bool D6Joint::IsEnableAlwaysUpdate() const {
    return m_impl->IsEnableAlwaysUpdate();
}

}  // namespace nickel::physics