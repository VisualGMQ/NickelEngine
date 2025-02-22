#include "nickel/physics/rigidbody.hpp"

#include "nickel/physics/internal/rigidbody_impl.hpp"

namespace nickel::physics {

RigidActor::RigidActor(RigidActorImpl* impl) : m_impl{impl} {}

RigidActor::RigidActor(const RigidActor& o) : m_impl{o.m_impl} {
    if (m_impl) {
        m_impl->IncRefcount();
    }
}

RigidActor::RigidActor(RigidActor&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

RigidActor& RigidActor::operator=(const RigidActor& o) {
    if (&o != this) {
        if (m_impl) {
            m_impl->DecRefcount();
        }
        m_impl = o.m_impl;
        if (m_impl) {
            m_impl->IncRefcount();
        }
    }
    return *this;
}

RigidActor& RigidActor::operator=(RigidActor&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

RigidActor::~RigidActor() {
    if (m_impl) {
        m_impl->DecRefcount();
    }
}

uint32_t RigidActor::GetShapeNum() const {
    return m_impl->GetShapeNum();
}

void RigidActor::SetGlobalTransform(const Vec3& position,
                                    const Quat& rotation) {
    m_impl->SetGlobalTransform(position, rotation);
}

Transform RigidActor::GetGlobalTransform() const {
    return m_impl->GetGlobalTransform();
}

void RigidActor::DisableGravity(bool disable) {
    m_impl->DisableGravity(disable);
}

bool RigidActor::IsEnableGravity() {
    return m_impl->IsEnableGravity();
}

void RigidActor::DisableSimulation(bool disable) {
    m_impl->DisableSimulation(disable);
}

bool RigidActor::IsEnableSimulation() {
    return m_impl->IsEnableSimulation();
}

void RigidActor::AttachShape(const Shape& shape) {
    m_impl->AttachShape(shape);
}

void RigidActor::DetachShape(const Shape& shape) {
    m_impl->DetachShape(shape);
}

RigidActorImpl* RigidActor::GetImpl() {
    return m_impl;
}

const RigidActorImpl* RigidActor::GetImpl() const {
    return m_impl;
}

RigidStaticImpl* RigidStatic::getUnderlyingImpl() {
    return static_cast<RigidStaticImpl*>(GetImpl());
}

const RigidStaticImpl* RigidStatic::getUnderlyingImpl() const {
    return static_cast<const RigidStaticImpl*>(GetImpl());
}

void RigidDynamic::SetKinematicTarget(const Vec3& force, const Quat& q) {
    getUnderlyingImpl()->SetKinematicTarget(force, q);
}

void RigidDynamic::EnableKinematic(bool enable) {
    getUnderlyingImpl()->EnableKinematic(enable);
}

bool RigidDynamic::IsKinematic() const {
    return getUnderlyingImpl()->IsKinematic();
}

void RigidDynamic::EnabelCCD(bool enable) {
    getUnderlyingImpl()->EnabelCCD(enable);
}

void RigidDynamic::EnableGyroscopicForces(bool enable) {
    getUnderlyingImpl()->EnableGyroscopicForces(enable);
}

void RigidDynamic::LockLinearX(bool lock) {
    getUnderlyingImpl()->LockLinearX(lock);
}

void RigidDynamic::LockLinearY(bool lock) {
    getUnderlyingImpl()->LockLinearY(lock);
}

void RigidDynamic::LockLinearZ(bool lock) {
    getUnderlyingImpl()->LockLinearZ(lock);
}

void RigidDynamic::LockAngularX(bool lock) {
    getUnderlyingImpl()->LockAngularX(lock);
}

void RigidDynamic::LockAngularY(bool lock) {
    getUnderlyingImpl()->LockAngularY(lock);
}

void RigidDynamic::LockAngularZ(bool lock) {
    getUnderlyingImpl()->LockAngularZ(lock);
}

bool RigidDynamic::IsLinearXLocked() const {
    return getUnderlyingImpl()->IsLinearXLocked();
}

bool RigidDynamic::IsLinearYLocked() const {
    return getUnderlyingImpl()->IsLinearYLocked();
}

bool RigidDynamic::IsLinearZLocked() const {
    return getUnderlyingImpl()->IsLinearZLocked();
}

bool RigidDynamic::IsAngularXLocked() const {
    return getUnderlyingImpl()->IsAngularXLocked();
}

bool RigidDynamic::IsAngularYLocked() const {
    return getUnderlyingImpl()->IsAngularYLocked();
}

bool RigidDynamic::IsAngularZLocked() const {
    return getUnderlyingImpl()->IsAngularZLocked();
}

void RigidDynamic::SetLinearVelocity(const Vec3& v) {
    getUnderlyingImpl()->SetLinearVelocity(v);
}

void RigidDynamic::SetAngularVelocity(const Vec3& v) {
    getUnderlyingImpl()->SetAngularVelocity(v);
}

Vec3 RigidDynamic::GetLinearVelocity() const {
    return getUnderlyingImpl()->GetLinearVelocity();
}

Vec3 RigidDynamic::GetAngularVelocity() const {
    return getUnderlyingImpl()->GetAngularVelocity();
}

void RigidDynamic::SetCenterOfMassLocalPose(const Vec3& p, const Quat& q) {
    getUnderlyingImpl()->SetCenterOfMassLocalPose(p, q);
}

Transform RigidDynamic::GetCenterOfMassLocalPose() const {
    return getUnderlyingImpl()->GetCenterOfMassLocalPose();
}

void RigidDynamic::SetMass(float m) {
    getUnderlyingImpl()->SetMass(m);
}

float RigidDynamic::GetMass() const {
    return getUnderlyingImpl()->GetMass();
}

void RigidDynamic::SetMassSpaceInertiaTensor(const Vec3& v) {
    getUnderlyingImpl()->SetMassSpaceInertiaTensor(v);
}

Vec3 RigidDynamic::GetMassSpaceInertiaTensor() const {
    return getUnderlyingImpl()->GetMassSpaceInertiaTensor();
}

void RigidDynamic::SetLinearDamping(float damping) {
    getUnderlyingImpl()->SetLinearDamping(damping);
}

float RigidDynamic::GetLinearDamping() const {
    return getUnderlyingImpl()->GetLinearDamping();
}

void RigidDynamic::SetAngularDamping(float damping) {
    getUnderlyingImpl()->SetAngularDamping(damping);
}

float RigidDynamic::GetAngularDamping() const {
    return getUnderlyingImpl()->GetAngularDamping();
}

void RigidDynamic::SetMaxLinearVelocity(float v) {
    getUnderlyingImpl()->SetMaxLinearVelocity(v);
}

float RigidDynamic::GetMaxLinearVelocity() {
    return getUnderlyingImpl()->GetMaxLinearVelocity();
}

void RigidDynamic::SetMaxAngularVelocity(float v) {
    getUnderlyingImpl()->SetMaxAngularVelocity(v);
}

float RigidDynamic::GetMaxAngularVelocity() {
    return getUnderlyingImpl()->GetMaxAngularVelocity();
}

void RigidDynamic::AddForce(const Vec3& f, ForceMode mode) {
    getUnderlyingImpl()->AddForce(f, mode);
}

void RigidDynamic::ClearForce(ForceMode mode) {
    getUnderlyingImpl()->ClearForce(mode);
}

void RigidDynamic::AddTorque(const Vec3& f, ForceMode mode) {
    getUnderlyingImpl()->AddTorque(f, mode);
}

void RigidDynamic::ClearTorque(ForceMode mode) {
    getUnderlyingImpl()->ClearTorque(mode);
}

void RigidDynamic::SetForceAndTorque(const Vec3& force, const Vec3& torque,
                                     ForceMode mode) {
    getUnderlyingImpl()->SetForceAndTorque(force, torque, mode);
}

RigidDynamicImpl* RigidDynamic::getUnderlyingImpl() {
    return static_cast<RigidDynamicImpl*>(RigidActor::GetImpl());
}

const RigidDynamicImpl* RigidDynamic::getUnderlyingImpl() const {
    return static_cast<const RigidDynamicImpl*>(RigidActor::GetImpl());
}

RigidActorConst::RigidActorConst(RigidActorConstImpl* impl) : m_impl{impl} {}

RigidActorConst::RigidActorConst(const RigidActorConst& o) : m_impl{o.m_impl} {
    if (m_impl) {
        m_impl->IncRefcount();
    }
}

RigidActorConst::RigidActorConst(RigidActorConst&& o) noexcept
    : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

RigidActorConst& RigidActorConst::operator=(const RigidActorConst& o) {
    if (&o != this) {
        if (m_impl) {
            m_impl->DecRefcount();
        }
        m_impl = o.m_impl;
        if (m_impl) {
            m_impl->IncRefcount();
        }
    }
    return *this;
}

RigidActorConst& RigidActorConst::operator=(RigidActorConst&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

RigidActorConst::~RigidActorConst() {
    if (m_impl) {
        m_impl->DecRefcount();
    }
}

Transform RigidActorConst::GetGlobalTransform() const {
    return m_impl->GetGlobalTransform();
}

uint32_t RigidActorConst::GetShapeNum() const {
    return m_impl->GetShapeNum();
}

RigidActorConstImpl* RigidActorConst::GetImpl() {
    return m_impl;
}

const RigidActorConstImpl* RigidActorConst::GetImpl() const {
    return m_impl;
}

}  // namespace nickel::physics