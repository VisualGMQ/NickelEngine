#include "nickel/physics/internal/context_impl.hpp"
#include "nickel/physics/internal/geometry_converter.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/internal/scene_impl.hpp"
#include "nickel/physics/internal/util.hpp"
#include "nickel/physics/scene.hpp"

namespace nickel::physics {

ContextImpl::ContextImpl() {
    m_foundation =
        PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_error_callback);
    if (!m_foundation) {
        LOGC("Failed to init PhysX");
    }

    m_physics =
        PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, m_tolerances_scale);
    if (!m_physics) {
        LOGC("Failed to create PxPhysics");
    }

    physx::PxSceneDesc desc{m_tolerances_scale};
    desc.gravity = Vec3ToPhysX(Vec3{0, -9.8f, 0});
    desc.frictionType = physx::PxFrictionType::ePATCH;
    desc.solverType = physx::PxSolverType::eTGS;
    desc.filterShader = physx::PxDefaultSimulationFilterShader;

    m_cpu_dispatcher = physx::PxDefaultCpuDispatcherCreate(4);
    desc.cpuDispatcher = m_cpu_dispatcher;
    m_main_scene = m_scene_allocator.Allocate("NickelMainPhysicsScene", this,
                                              m_physics->createScene(desc));

    m_blast_framework = NvBlastTkFrameworkCreate();
}

ContextImpl::~ContextImpl() {
    m_material_allocator.FreeAll();
    m_shape_allocator.FreeAll();
    m_rigid_actor_allocator.FreeAll();
    m_scene_allocator.FreeAll();
    m_blast_framework->release();
    m_physics->release();
    m_foundation->release();
}

Scene ContextImpl::CreateScene(const std::string& name, const Vec3& gravity) {
    physx::PxSceneDesc desc{m_tolerances_scale};
    desc.gravity = Vec3ToPhysX(Vec3{0, -9.8f, 0});
    desc.frictionType = physx::PxFrictionType::ePATCH;
    desc.solverType = physx::PxSolverType::eTGS;
    desc.filterShader = physx::PxDefaultSimulationFilterShader;

    m_cpu_dispatcher = physx::PxDefaultCpuDispatcherCreate(4);
    desc.cpuDispatcher = m_cpu_dispatcher;
    return m_scene_allocator.Allocate(name, this, m_physics->createScene(desc));
}

Material ContextImpl::CreateMaterial(float static_friction,
                                     float dynamic_friction,
                                     float restitution) {
    physx::PxMaterial* mtl = m_physics->createMaterial(
        static_friction, dynamic_friction, restitution);
    if (!mtl) {
        return {};
    }
    return m_material_allocator.Allocate(this, mtl);
}

RigidStatic ContextImpl::CreateRigidStatic(const Vec3& p, const Quat& q) {
    physx::PxRigidStatic* rigid =
        m_physics->createRigidStatic({Vec3ToPhysX(p), QuatToPhysX(q)});
    if (!rigid) {
        return {};
    }
    return m_rigid_actor_allocator.Allocate(
        this, static_cast<physx::PxRigidActor*>(rigid));
}

RigidDynamic ContextImpl::CreateRigidDynamic(const Vec3& p, const Quat& q) {
    physx::PxRigidDynamic* rigid =
        m_physics->createRigidDynamic({Vec3ToPhysX(p), QuatToPhysX(q)});
    if (!rigid) {
        return {};
    }
    return m_rigid_actor_allocator.Allocate(
        this, static_cast<physx::PxRigidActor*>(rigid));
}

TriangleMesh ContextImpl::CreateTriangleMesh(const Vec3* vertices,
                                             uint32_t vertex_count,
                                             const uint32_t* indices,
                                             uint32_t index_count) {
    physx::PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = vertex_count;
    meshDesc.points.stride = sizeof(physx::PxVec3);
    meshDesc.points.data = vertices;

    meshDesc.triangles.count = index_count;
    meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
    meshDesc.triangles.data = indices;

    physx::PxCookingParams params(m_tolerances_scale);

    physx::PxDefaultMemoryOutputStream writeBuffer;
    physx::PxTriangleMeshCookingResult::Enum result;
    bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);
    if (!status) {
        return NULL;
    }

    physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(),
                                               writeBuffer.getSize());
    return m_physics->createTriangleMesh(readBuffer);
}

Shape ContextImpl::CreateShape(const SphereGeometry& geom,
                               const Material& material) {
    physx::PxShape* shape = m_physics->createShape(Geometry2PhysX(geom),
                                                   *material.GetImpl()->m_mtl);
    if (shape) {
        return Shape{m_shape_allocator.Allocate(this, shape)};
    }
    return {};
}

Shape ContextImpl::CreateShape(const BoxGeometry& geom,
                               const Material& material) {
    physx::PxShape* shape = m_physics->createShape(Geometry2PhysX(geom),
                                                   *material.GetImpl()->m_mtl);
    if (shape) {
        return Shape{m_shape_allocator.Allocate(this, shape)};
    }
    return {};
}

Shape ContextImpl::CreateShape(const CapsuleGeometry& geom,
                               const Material& material) {
    physx::PxShape* shape = m_physics->createShape(Geometry2PhysX(geom),
                                                   *material.GetImpl()->m_mtl);
    if (shape) {
        return Shape{m_shape_allocator.Allocate(this, shape)};
    }
    return {};
}

Shape ContextImpl::CreateShape(const TriangleMeshGeometry& geom,
                               const Material& material) {
    physx::PxShape* shape = m_physics->createShape(Geometry2PhysX(geom),
                                                   *material.GetImpl()->m_mtl);
    if (shape) {
        return Shape{m_shape_allocator.Allocate(this, shape)};
    }
    return {};
}

Shape ContextImpl::CreateShape(const PlaneGeometry& geom,
                               const Material& material) {
    physx::PxShape* shape = m_physics->createShape(Geometry2PhysX(geom),
                                                   *material.GetImpl()->m_mtl);
    if (shape) {
        return Shape{m_shape_allocator.Allocate(this, shape)};
    }
    return {};
}

void ContextImpl::Update(float delta_time) {
    m_main_scene->Simulate(delta_time);
}

Scene ContextImpl::GetMainScene() {
    return Scene{m_main_scene};
}

void ContextImpl::GC() {
    m_shape_allocator.GC();
    m_material_allocator.GC();
    m_rigid_actor_allocator.GC();
    m_scene_allocator.GC();
}

}  // namespace nickel::physics