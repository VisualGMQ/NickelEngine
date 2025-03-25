#pragma once
#include "NvBlastTk.h"
#include "nickel/common/assert.hpp"
#include "nickel/common/math/math.hpp"
#include "nickel/common/memory/memory.hpp"
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
#include "nickel/physics/vehicle.hpp"

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

struct QueryFilterCallback: public physx::PxQueryFilterCallback {
    physx::PxQueryHitType::Enum preFilter(
        const physx::PxFilterData& filterData, const physx::PxShape* shape,
        const physx::PxRigidActor* actor,
        physx::PxHitFlags& queryFlags) override;
    physx::PxQueryHitType::Enum postFilter(
        const physx::PxFilterData& filterData, const physx::PxQueryHit& hit,
        const physx::PxShape* shape, const physx::PxRigidActor* actor) override;
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
    
    Shape CreateShape(const SphereGeometry&, const Material&, bool is_exclusive = false);
    Shape CreateShape(const BoxGeometry&, const Material&, bool is_exclusive = false);
    Shape CreateShape(const CapsuleGeometry&, const Material&, bool is_exclusive = false);
    Shape CreateShape(const TriangleMeshGeometry&, const Material&, bool is_exclusive = false);
    Shape CreateShape(const ConvexMeshGeometry&, const Material&, bool is_exclusive = false);
    Shape CreateShape(const PlaneGeometry&, const Material&, bool is_exclusive = false);

    D6Joint CreateD6Joint(const RigidActor& actor0, const Vec3& p0,
                          const Quat& q0, const RigidActor& actor1,
                          const Vec3& p1, const Quat& q1);

    void Update(float delta_time);

    Scene GetMainScene();
    VehicleManager& GetVehicleManager();
    const VehicleManager& GetVehicleManager() const;

    void GC();

    physx::PxTolerancesScale m_tolerances_scale;
    physx::PxPhysics* m_physics;
    QueryFilterCallback m_query_filter_callback;

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
    std::unique_ptr<VehicleManager> m_vehicle_manager;
    physx::PxPvd* m_pvd;
    physx::PxPvdTransport* m_pvd_transport;

    static physx::PxFilterFlags SimulateFilterShader(
        physx::PxFilterObjectAttributes attributes0,
        physx::PxFilterData filterData0,
        physx::PxFilterObjectAttributes attributes1,
        physx::PxFilterData filterData1, physx::PxPairFlags& pairFlags,
        const void* constantBlock, physx::PxU32 constantBlockSize);

    Scene m_main_scene;
    Nv::Blast::TkFramework* m_blast_framework;
};

}  // namespace nickel::physics