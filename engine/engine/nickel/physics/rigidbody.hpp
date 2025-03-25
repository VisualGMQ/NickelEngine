#pragma once
#include "nickel/common/transform.hpp"
#include "nickel/physics/enums.hpp"
#include "nickel/physics/shape.hpp"

namespace nickel::physics {

class RigidActorImpl;
class RigidActorConstImpl;
class RigidDynamicImpl;
class RigidStaticImpl;

class RigidActor: public ImplWrapper<RigidActorImpl> {
public:
    using ImplWrapper::ImplWrapper;

    void SetGlobalTransform(const Vec3& position, const Quat& rotation);
    Transform GetGlobalTransform() const;

    void DisableGravity(bool disable);
    bool IsEnableGravity() const;
    void DisableSimulation(bool disable);
    bool IsEnableSimulation() const;

    void AttachShape(Shape& shape);
    void DetachShape(const Shape& shape);
    uint32_t GetShapeNum() const;
    std::vector<Shape> GetShapes() const;
};

class RigidStatic : public RigidActor {
public:
    using RigidActor::RigidActor;

    operator RigidActor();

private:
    RigidStaticImpl* getUnderlyingImpl();
    const RigidStaticImpl* getUnderlyingImpl() const;
};

class RigidDynamic : public RigidActor {
public:
    using RigidActor::RigidActor;

    operator RigidActor();

    void SetKinematicTarget(const Vec3& force, const Quat& q);
    void EnableKinematic(bool enable);
    bool IsKinematic() const;
    void EnableCCD(bool);
    bool IsEnableCCD() const;
    void EnableGyroscopicForces(bool enable);
    bool IsEnableGyroscopicForces() const;

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

class RigidActorConst : public ImplWrapper<RigidActorConstImpl> {
public:
    using ImplWrapper::ImplWrapper;

    Transform GetGlobalTransform() const;

    uint32_t GetShapeNum() const;
};

}  // namespace nickel::physics