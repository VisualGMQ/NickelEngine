#pragma once
#include "nickel/common/math/math.hpp"
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/common/transform.hpp"
#include "nickel/physics/enums.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/shape.hpp"

namespace nickel::physics {

class ContextImpl;

class RigidActorImpl : public RefCountable {
public:
    RigidActorImpl() = default;
    RigidActorImpl(ContextImpl*, physx::PxRigidActor*);
    ~RigidActorImpl();
    void DecRefcount() override;

    RigidActorType GetType() const;

    void SetGlobalTransform(const Vec3& position, const Quat& rotation);
    Transform GetGlobalTransform() const;

    void DisableGravity(bool disable);
    bool IsEnableGravity();
    void DisableSimulation(bool disable);
    bool IsEnableSimulation();
    
    void AttachShape(const Shape& shape);
    void DetachShape(const Shape& shape);
    uint32_t GetShapeNum() const;
    std::vector<Shape> GetShapes() const;

    physx::PxRigidActor* m_actor{};

    ContextImpl* m_ctx{};
};

class RigidStaticImpl : public RigidActorImpl {
public:

private:
    physx::PxRigidStatic* getUnderlying();
    const physx::PxRigidStatic* getUnderlying() const;
};

class RigidDynamicImpl : public RigidActorImpl {
public:
    void SetKinematicTarget(const Vec3& force, const Quat& q);
    void EnableKinematic(bool enable);
    bool IsKinematic() const;
    void EnabelCCD(bool);
    void EnableGyroscopicForces(bool enable);

    void LockLinearX(bool lock);
    void LockLinearY(bool lock);
    void LockLinearZ(bool lock);
    void LockAngularX(bool lock);
    void LockAngularY(bool lock);
    void LockAngularZ(bool lock);

    bool IsLinearXLocked() const;
    bool IsLinearYLocked() const;
    bool IsLinearZLocked() const;
    bool IsAngularXLocked() const;
    bool IsAngularYLocked() const;
    bool IsAngularZLocked() const;

    void SetLinearVelocity(const Vec3&);
    void SetAngularVelocity(const Vec3&);
    Vec3 GetLinearVelocity() const;
    Vec3 GetAngularVelocity() const;

    void SetCenterOfMassLocalPose(const Vec3& p, const Quat& q);
    Transform GetCenterOfMassLocalPose() const;
    void SetMass(float);
    float GetMass() const;
    void SetMassSpaceInertiaTensor(const Vec3&);
    Vec3 GetMassSpaceInertiaTensor() const;

    void SetLinearDamping(float);
    float GetLinearDamping() const;
    void SetAngularDamping(float);
    float GetAngularDamping() const;

    void SetMaxLinearVelocity(float);
    float GetMaxLinearVelocity();
    void SetMaxAngularVelocity(float);
    float GetMaxAngularVelocity();

    void AddForce(const Vec3& f, ForceMode);
    void ClearForce(ForceMode = ForceMode::Force);
    void AddTorque(const Vec3& f, ForceMode);
    void ClearTorque(ForceMode = ForceMode::Force);
    void SetForceAndTorque(const Vec3& force, const Vec3& torque, ForceMode);

private:
    physx::PxRigidDynamic* getUnderlying();
    const physx::PxRigidDynamic* getUnderlying() const;
};

class RigidActorConstImpl : protected RigidActorImpl{
public:
    RigidActorConstImpl() = default;
    RigidActorConstImpl(ContextImpl*, const physx::PxRigidActor*);

    using RefCountable::IncRefcount;
    void DecRefcount() override;

    using RigidActorImpl::GetType;
    using RigidActorImpl::GetGlobalTransform;
    using RigidActorImpl::GetShapeNum;
};


}  // namespace nickel::physics