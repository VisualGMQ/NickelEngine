#pragma once
#include "nickel/common/transform.hpp"
#include "nickel/physics/enums.hpp"
#include "nickel/physics/shape.hpp"

namespace nickel::physics {

class RigidActorImpl;
class RigidActorConstImpl;
class RigidDynamicImpl;
class RigidStaticImpl;

class RigidActor {
public:
    RigidActor() = default;
    RigidActor(RigidActorImpl*);
    RigidActor(const RigidActor&);
    RigidActor(RigidActor&&) noexcept;
    RigidActor& operator=(const RigidActor&);
    RigidActor& operator=(RigidActor&&) noexcept;
    virtual ~RigidActor();

    void SetGlobalTransform(const Vec3& position, const Quat& rotation);
    Transform GetGlobalTransform() const;

    void DisableGravity(bool disable);
    bool IsEnableGravity();
    void DisableSimulation(bool disable);
    bool IsEnableSimulation();

    void AttachShape(const Shape& shape);
    void DetachShape(const Shape& shape);
    uint32_t GetShapeNum() const;

    RigidActorImpl* GetImpl();
    const RigidActorImpl* GetImpl() const;

private:
    RigidActorImpl* m_impl{};
};

class RigidStatic : public RigidActor {
public:
    using RigidActor::RigidActor;

private:
    RigidStaticImpl* getUnderlyingImpl();
    const RigidStaticImpl* getUnderlyingImpl() const;
};

class RigidDynamic : public RigidActor {
public:
    using RigidActor::RigidActor;

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
    RigidDynamicImpl* getUnderlyingImpl();
    const RigidDynamicImpl* getUnderlyingImpl() const;
};

class RigidActorConst {
public:
    RigidActorConst() = default;
    RigidActorConst(RigidActorConstImpl*);
    RigidActorConst(const RigidActorConst&);
    RigidActorConst(RigidActorConst&&) noexcept;
    RigidActorConst& operator=(const RigidActorConst&);
    RigidActorConst& operator=(RigidActorConst&&) noexcept;
    virtual ~RigidActorConst();

    Transform GetGlobalTransform() const;

    uint32_t GetShapeNum() const;

    RigidActorConstImpl* GetImpl();
    const RigidActorConstImpl* GetImpl() const;

private:
    RigidActorConstImpl* m_impl{};
};


}  // namespace nickel::physics