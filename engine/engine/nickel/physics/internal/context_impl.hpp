#pragma once
#include "NvBlastTk.h"
#include "nickel/common/assert.hpp"
#include "nickel/common/math/math.hpp"
#include "nickel/common/memory/memory.hpp"
#include "nickel/physics/context.hpp"
#include "nickel/physics/geometry.hpp"
#include "nickel/physics/internal/joint_impl.hpp"
#include "nickel/physics/internal/material_impl.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/internal/rigidbody_impl.hpp"
#include "nickel/physics/internal/scene_impl.hpp"
#include "nickel/physics/internal/shape_impl.hpp"
#include "nickel/physics/material.hpp"
#include "nickel/physics/rigidbody.hpp"
#include "nickel/physics/scene.hpp"
#include "nickel/physics/shape.hpp"

namespace nickel::physics {

class PhysXErrorCallback : public physx::PxErrorCallback {
public:
    virtual void reportError(physx::PxErrorCode::Enum code, const char* message,
                             const char* file, int line) {
        constexpr const char* log_tmpl = "[PhysX][{}]({} {}ln): {}";
        switch (code) {
            case physx::PxErrorCode::eNO_ERROR:
                return;
            case physx::PxErrorCode::eDEBUG_INFO:
                LOGD(log_tmpl, "", file, line, message);
                return;
            case physx::PxErrorCode::eDEBUG_WARNING:
                LOGW(log_tmpl, "", file, line, message);
                return;
            case physx::PxErrorCode::eINVALID_PARAMETER:
                LOGW(log_tmpl, "Invalid Parameter", file, line, message);
                return;
            case physx::PxErrorCode::eINVALID_OPERATION:
                LOGW(log_tmpl, "Invalid Operation", file, line, message);
                return;
            case physx::PxErrorCode::eOUT_OF_MEMORY:
                LOGW(log_tmpl, "Out Of Memory", file, line, message);
                return;
            case physx::PxErrorCode::eINTERNAL_ERROR:
                LOGE(log_tmpl, "Internal Error", file, line, message);
                return;
            case physx::PxErrorCode::eABORT:
                LOGC(log_tmpl, "", file, line, message);
                return;
            case physx::PxErrorCode::ePERF_WARNING:
                LOGW(log_tmpl, "Performance", file, line, message);
                return;
        }

        NICKEL_CANT_REACH();
    }
};

class ContextImpl {
public:
    ContextImpl();
    ~ContextImpl();

    Scene CreateScene(const std::string& name, const Vec3& gravity);
    Material CreateMaterial(float static_friction, float dynamic_friction,
                            float restitution);
    RigidStatic CreateRigidStatic(const Vec3& p, const Quat& q);
    RigidDynamic CreateRigidDynamic(const Vec3& p, const Quat& q);
    TriangleMesh CreateTriangleMesh(std::span<const Vec3> vertices,
                                    std::span<const uint32_t> indices);
    ConvexMesh CreateConvexMesh(std::span<const Vec3> vertices);
    
    Shape CreateShape(const SphereGeometry&, const Material&);
    Shape CreateShape(const BoxGeometry&, const Material&);
    Shape CreateShape(const CapsuleGeometry&, const Material&);
    Shape CreateShape(const TriangleMeshGeometry&, const Material&);
    Shape CreateShape(const ConvexMeshGeometry&, const Material&);
    Shape CreateShape(const PlaneGeometry&, const Material&);

    D6Joint CreateD6Joint(const RigidActor& actor0, const Vec3& p0,
                          const Quat& q0, const RigidActor& actor1,
                          const Vec3& p1, const Quat& q1);

    void Update(float delta_time);

    Scene GetMainScene();

    void GC();

    physx::PxTolerancesScale m_tolerances_scale;
    physx::PxPhysics* m_physics;

    BlockMemoryAllocator<SceneImpl> m_scene_allocator;
    BlockMemoryAllocator<RigidActorImpl> m_rigid_actor_allocator;
    BlockMemoryAllocator<RigidActorConstImpl> m_rigid_actor_const_allocator;
    BlockMemoryAllocator<MaterialImpl> m_material_allocator;
    BlockMemoryAllocator<ShapeImpl> m_shape_allocator;
    BlockMemoryAllocator<ShapeConstImpl> m_shape_const_allocator;
    BlockMemoryAllocator<D6JointImpl> m_joint_allocator;

private:
    physx::PxFoundation* m_foundation;
    PhysXErrorCallback m_error_callback;
    physx::PxDefaultAllocator m_allocator;
    physx::PxDefaultCpuDispatcher* m_cpu_dispatcher;

    SceneImpl* m_main_scene;
    Nv::Blast::TkFramework* m_blast_framework;
};

}  // namespace nickel::physics