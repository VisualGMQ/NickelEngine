#include "nickel/physics/internal/rigidbody_impl.hpp"

#include "nickel/physics/internal/context_impl.hpp"
#include "nickel/physics/internal/enum_convert.hpp"
#include "nickel/physics/internal/shape_impl.hpp"
#include "nickel/physics/internal/util.hpp"

namespace nickel::physics {

RigidActorImpl::RigidActorImpl(ContextImpl* ctx, physx::PxRigidActor* actor)
    : m_actor{actor}, m_ctx{ctx} {}

RigidActorImpl::~RigidActorImpl() {
    if (m_actor) {
        m_actor->release();
    }
}

void RigidActorImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0 && m_actor && m_ctx) {
        m_ctx->m_rigid_actor_allocator.MarkAsGarbage(this);
    }
}

RigidActorType RigidActorImpl::GetType() const {
    return RigidActorTypeFromPhysX(m_actor->getType());
}

uint32_t RigidActorImpl::GetShapeNum() const {
    return m_actor->getNbShapes();
}

void RigidActorImpl::SetGlobalTransform(const Vec3& position,
                                        const Quat& rotation) {
    m_actor->setGlobalPose(
        physx::PxTransform{Vec3ToPhysX(position), QuatToPhysX(rotation)});
}

Transform RigidActorImpl::GetGlobalTransform() const {
    auto t = m_actor->getGlobalPose();
    return TransformFromPhysX(t);
}

void RigidActorImpl::DisableGravity(bool disable) {
    m_actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, disable);
}

bool RigidActorImpl::IsEnableGravity() {
    return !(m_actor->getActorFlags() & physx::PxActorFlag::eDISABLE_GRAVITY);
}

void RigidActorImpl::DisableSimulation(bool disable) {
    m_actor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, disable);
}

bool RigidActorImpl::IsEnableSimulation() {
    return !(m_actor->getActorFlags() &
             physx::PxActorFlag::eDISABLE_SIMULATION);
}

void RigidActorImpl::AttachShape(const Shape& shape) {
    m_actor->attachShape(*shape.m_impl->m_shape);
}

void RigidActorImpl::DetachShape(const Shape& shape) {
    m_actor->detachShape(*shape.m_impl->m_shape);
}

physx::PxRigidStatic* RigidStaticImpl::getUnderlying() {
    return static_cast<physx::PxRigidStatic*>(m_actor);
}

const physx::PxRigidStatic* RigidStaticImpl::getUnderlying() const {
    return static_cast<const physx::PxRigidStatic*>(m_actor);
}

void RigidDynamicImpl::SetMass(float m) {
    getUnderlying()->setMass(m);
}

float RigidDynamicImpl::GetMass() const {
    return getUnderlying()->getMass();
}

void RigidDynamicImpl::SetMassSpaceInertiaTensor(const Vec3& v) {
    getUnderlying()->setMassSpaceInertiaTensor(Vec3ToPhysX(v));
}

Vec3 RigidDynamicImpl::GetMassSpaceInertiaTensor() const {
    return Vec3FromPhysX(getUnderlying()->getMassSpaceInertiaTensor());
}

void RigidDynamicImpl::SetLinearDamping(float co) {
    getUnderlying()->setLinearDamping(co);
}

float RigidDynamicImpl::GetLinearDamping() const {
    return getUnderlying()->getLinearDamping();
}

float RigidDynamicImpl::GetAngularDamping() const {
    return getUnderlying()->getAngularDamping();
}

void RigidDynamicImpl::SetMaxLinearVelocity(float v) {
    getUnderlying()->setMaxLinearVelocity(v);
}

float RigidDynamicImpl::GetMaxLinearVelocity() {
    return getUnderlying()->getMaxLinearVelocity();
}

void RigidDynamicImpl::SetMaxAngularVelocity(float v) {
    getUnderlying()->setMaxAngularVelocity(v);
}

float RigidDynamicImpl::GetMaxAngularVelocity() {
    return getUnderlying()->getMaxAngularVelocity();
}

void RigidDynamicImpl::AddForce(const Vec3& f, ForceMode mode) {
    getUnderlying()->addForce(Vec3ToPhysX(f), ForceMode2PhysX(mode));
}

void RigidDynamicImpl::ClearForce(ForceMode mode) {
    getUnderlying()->clearForce(ForceMode2PhysX(mode));
}

void RigidDynamicImpl::AddTorque(const Vec3& f, ForceMode mode) {
    getUnderlying()->addTorque(Vec3ToPhysX(f), ForceMode2PhysX(mode));
}

void RigidDynamicImpl::ClearTorque(ForceMode mode) {
    getUnderlying()->clearTorque(ForceMode2PhysX(mode));
}

void RigidDynamicImpl::SetAngularDamping(float co) {
    getUnderlying()->setAngularDamping(co);
}

void RigidDynamicImpl::SetForceAndTorque(const Vec3& force, const Vec3& torque,
                                         ForceMode mode) {
    getUnderlying()->setForceAndTorque(Vec3ToPhysX(force), Vec3ToPhysX(torque),
                                       ForceMode2PhysX(mode));
}

void RigidDynamicImpl::SetKinematicTarget(const Vec3& force, const Quat& q) {
    getUnderlying()->setKinematicTarget({Vec3ToPhysX(force), QuatToPhysX(q)});
}

void RigidDynamicImpl::EnableKinematic(bool enable) {
    getUnderlying()->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC,
                                      enable);
}

bool RigidDynamicImpl::IsKinematic() const {
    return getUnderlying()->getRigidBodyFlags() &
           physx::PxRigidBodyFlag::eKINEMATIC;
}

void RigidDynamicImpl::EnabelCCD(bool enable) {
    getUnderlying()->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD,
                                      enable);
}

void RigidDynamicImpl::EnableGyroscopicForces(bool enable) {
    getUnderlying()->setRigidBodyFlag(
        physx::PxRigidBodyFlag::eENABLE_GYROSCOPIC_FORCES, enable);
}

void RigidDynamicImpl::LockLinearX(bool lock) {
    getUnderlying()->setRigidDynamicLockFlag(
        physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, lock);
}

void RigidDynamicImpl::LockLinearY(bool lock) {
    getUnderlying()->setRigidDynamicLockFlag(
        physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, lock);
}

void RigidDynamicImpl::LockLinearZ(bool lock) {
    getUnderlying()->setRigidDynamicLockFlag(
        physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, lock);
}

void RigidDynamicImpl::LockAngularX(bool lock) {
    getUnderlying()->setRigidDynamicLockFlag(
        physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, lock);
}

void RigidDynamicImpl::LockAngularY(bool lock) {
    getUnderlying()->setRigidDynamicLockFlag(
        physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, lock);
}

void RigidDynamicImpl::LockAngularZ(bool lock) {
    getUnderlying()->setRigidDynamicLockFlag(
        physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, lock);
}

bool RigidDynamicImpl::IsLinearXLocked() const {
    return getUnderlying()->getRigidDynamicLockFlags() &
           physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
}

bool RigidDynamicImpl::IsLinearYLocked() const {
    return getUnderlying()->getRigidDynamicLockFlags() &
           physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;
}

bool RigidDynamicImpl::IsLinearZLocked() const {
    return getUnderlying()->getRigidDynamicLockFlags() &
           physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;
}

bool RigidDynamicImpl::IsAngularXLocked() const {
    return getUnderlying()->getRigidDynamicLockFlags() &
           physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
}

bool RigidDynamicImpl::IsAngularYLocked() const {
    return getUnderlying()->getRigidDynamicLockFlags() &
           physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
}

bool RigidDynamicImpl::IsAngularZLocked() const {
    return getUnderlying()->getRigidDynamicLockFlags() &
           physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
}

void RigidDynamicImpl::SetLinearVelocity(const Vec3& v) {
    getUnderlying()->setLinearVelocity(Vec3ToPhysX(v));
}

void RigidDynamicImpl::SetAngularVelocity(const Vec3& v) {
    getUnderlying()->setAngularVelocity(Vec3ToPhysX(v));
}

Vec3 RigidDynamicImpl::GetLinearVelocity() const {
    return Vec3FromPhysX(getUnderlying()->getLinearVelocity());
}

Vec3 RigidDynamicImpl::GetAngularVelocity() const {
    return Vec3FromPhysX(getUnderlying()->getAngularVelocity());
}

void RigidDynamicImpl::SetCenterOfMassLocalPose(const Vec3& p, const Quat& q) {
    getUnderlying()->setCMassLocalPose({Vec3ToPhysX(p), QuatToPhysX(q)});
}

Transform RigidDynamicImpl::GetCenterOfMassLocalPose() const {
    return TransformFromPhysX(getUnderlying()->getCMassLocalPose());
}

physx::PxRigidDynamic* RigidDynamicImpl::getUnderlying() {
    return static_cast<physx::PxRigidDynamic*>(m_actor);
}

const physx::PxRigidDynamic* RigidDynamicImpl::getUnderlying() const {
    return static_cast<physx::PxRigidDynamic*>(m_actor);
}

}  // namespace nickel::physics