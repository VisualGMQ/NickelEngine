#include "nickel/physics/internal/context_impl.hpp"
#include "nickel/common/bit_manipulate.hpp"
#include "nickel/physics/internal/enum_convert.hpp"
#include "nickel/physics/internal/geometry_converter.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/internal/scene_impl.hpp"
#include "nickel/physics/internal/util.hpp"
#include "nickel/physics/scene.hpp"

namespace nickel::physics {

physx::PxQueryHitType::Enum QueryFilterCallback::preFilter(
    const physx::PxFilterData& query_data, const physx::PxShape* shape,
    const physx::PxRigidActor* actor, physx::PxHitFlags& queryFlags) {
    auto filter_data = shape->getQueryFilterData();
    uint32_t collision_group_in_bit =
        1 << static_cast<std::underlying_type_t<CollisionGroup>>(
            filter_data.word0);
    if (collision_group_in_bit & query_data.word0) {
        // TODO: check block or touch
        return physx::PxQueryHitType::eTOUCH;
    }
    return physx::PxQueryHitType::eNONE;
}

physx::PxQueryHitType::Enum QueryFilterCallback::postFilter(
    const physx::PxFilterData& filterData, const physx::PxQueryHit& hit,
    const physx::PxShape* shape, const physx::PxRigidActor* actor) {
    return physx::PxQueryHitType::eTOUCH;
}

ContextImpl::ContextImpl() {
    m_foundation =
        PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_error_callback);
    if (!m_foundation) {
        LOGC("Failed to init PhysX");
    }

    m_pvd = physx::PxCreatePvd(*m_foundation);
    m_pvd_transport =
        physx::PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
    m_pvd->connect(*m_pvd_transport, physx::PxPvdInstrumentationFlag::eALL);

    m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation,
                                m_tolerances_scale, false, m_pvd);
    if (!m_physics) {
        LOGC("Failed to create PxPhysics");
    }

    if (!PxInitVehicleSDK(*m_physics)) {
        LOGE("Vehicle system init failed!");
    }

    physx::PxVehicleSetBasisVectors({0, 1, 0}, {1, 0, 0});
    physx::PxVehicleSetUpdateMode(physx::PxVehicleUpdateMode::eVELOCITY_CHANGE);

    m_main_scene = CreateScene("NickelEngineMainScene", {0, -9.8, 0});

    m_blast_framework = NvBlastTkFrameworkCreate();

    m_vehicle_manager =
        std::make_unique<VehicleManager>(*this, *m_main_scene.GetImpl());
}

ContextImpl::~ContextImpl() {
    m_vehicle_manager.reset();
    m_joint_allocator.FreeAll();
    m_material_allocator.FreeAll();
    m_shape_allocator.FreeAll();
    m_shape_const_allocator.FreeAll();
    m_rigid_actor_allocator.FreeAll();
    m_rigid_actor_const_allocator.FreeAll();
    m_scene_allocator.FreeAll();
    physx::PxCloseVehicleSDK();
    m_blast_framework->release();
    m_physics->release();
    m_pvd->release();
    m_pvd_transport->release();
    m_foundation->release();
}

physx::PxFilterFlags ContextImpl::SimulateFilterShader(
    physx::PxFilterObjectAttributes attributes0,
    physx::PxFilterData filterData0,
    physx::PxFilterObjectAttributes attributes1,
    physx::PxFilterData filterData1, physx::PxPairFlags& pairFlags,
    const void* constantBlock, physx::PxU32 constantBlockSize) {
    PX_UNUSED(constantBlock);
    PX_UNUSED(constantBlockSize);

    bool k0 = physx::PxFilterObjectIsKinematic(attributes0);
    bool k1 = physx::PxFilterObjectIsKinematic(attributes1);

    physx::PxU32 FilterFlags0 =
        filterData0.word0 >> SimulationBehaviorBitOffset;
    physx::PxU32 FilterFlags1 =
        filterData1.word0 >> SimulationBehaviorBitOffset;

    // TODO: implement after do contact modify
    // if (k0 && k1) {
    //     if (!(FilterFlags0 & EPDF_KinematicKinematicPairs) &&
    //         !(FilterFlags1 & EPDF_KinematicKinematicPairs)) {
    //         return physx::PxFilterFlag::eSUPPRESS;
    //     }
    // }

    bool s0 = physx::PxGetFilterObjectType(attributes0) ==
              physx::PxFilterObjectType::eRIGID_STATIC;
    bool s1 = physx::PxGetFilterObjectType(attributes1) ==
              physx::PxFilterObjectType::eRIGID_STATIC;

    if ((k0 || k1) && (s0 || s1)) {
        return physx::PxFilterFlag::eSUPPRESS;
    }

    // Find out which channels the objects are in
    std::underlying_type_t<CollisionGroup> Channel0 =
        filterData0.word0 & GenFullBitMast(SimulationBehaviorContainBits);
    std::underlying_type_t<CollisionGroup> Channel1 =
        filterData1.word0 & GenFullBitMast(SimulationBehaviorContainBits);

    // see if 0/1 would like to block the other
    bool block_flag_to_1 = (1 << Channel1) & filterData0.word2;
    bool block_flag_to0 = (1 << Channel0) & filterData1.word2;

    bool should_block = block_flag_to_1 && block_flag_to0;

    if (!should_block) {
        return physx::PxFilterFlag::eSUPPRESS;
    }

    pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT |
                physx::PxPairFlag::eDETECT_CCD_CONTACT |
                physx::PxPairFlag::eSOLVE_CONTACT;

    if ((FilterFlags0 & SimulationBehavior_Notify) ||
        (FilterFlags1 & SimulationBehavior_Notify)) {
        pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND |
                     physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS |
                     physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
    }

    if ((FilterFlags0 & SimulationBehavior_ModifyContacts) ||
        (FilterFlags1 & SimulationBehavior_ModifyContacts)) {
        pairFlags |= physx::PxPairFlag::eMODIFY_CONTACTS;
    }

    return physx::PxFilterFlags();
}

Scene ContextImpl::CreateScene(const std::string& name, const Vec3& gravity) {
    physx::PxSceneDesc desc{m_tolerances_scale};
    desc.gravity = Vec3ToPhysX(gravity);
    desc.frictionType = physx::PxFrictionType::ePATCH;
    desc.solverType = physx::PxSolverType::eTGS;
    desc.filterShader = SimulateFilterShader;
    desc.flags |= physx::PxSceneFlag::eENABLE_CCD;

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

TriangleMesh ContextImpl::CreateTriangleMesh(
    std::span<const Vec3> vertices, std::span<const uint32_t> indices) {
    physx::PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = vertices.size();
    meshDesc.points.stride = sizeof(physx::PxVec3);
    meshDesc.points.data = vertices.data();

    meshDesc.triangles.count = indices.size();
    meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
    meshDesc.triangles.data = indices.data();

    physx::PxCookingParams params(m_tolerances_scale);

    physx::PxDefaultMemoryOutputStream writeBuffer;
    physx::PxTriangleMeshCookingResult::Enum result;
    bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);
    if (!status) {
        return NULL;
    }
    switch (result) {
        case physx::PxTriangleMeshCookingResult::eLARGE_TRIANGLE:
            LOGW("cook triangle mesh faild: large triangle");
            break;
        case physx::PxTriangleMeshCookingResult::eEMPTY_MESH:
            LOGE("cook triangle mesh faild: empty mesh");
            break;
        case physx::PxTriangleMeshCookingResult::eFAILURE:
            LOGE("cook triangle mesh faild: failure");
            break;
        default:;
    }

    physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(),
                                               writeBuffer.getSize());
    return m_physics->createTriangleMesh(readBuffer);
}

ConvexMesh ContextImpl::CreateConvexMesh(std::span<const Vec3> vertices) {
    physx::PxConvexMeshDesc convexDesc;
    convexDesc.points.count = vertices.size();
    convexDesc.points.stride = sizeof(physx::PxVec3);
    convexDesc.points.data = vertices.data();
    convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

    physx::PxTolerancesScale scale;
    physx::PxCookingParams params(scale);

    physx::PxDefaultMemoryOutputStream buf;
    physx::PxConvexMeshCookingResult::Enum result;
    bool status = PxCookConvexMesh(params, convexDesc, buf, &result);
    if (!status) {
        return NULL;
    }
    switch (result) {
        case physx::PxConvexMeshCookingResult::eZERO_AREA_TEST_FAILED:
            LOGE("cook convex mesh faild: zero area");
            break;
        case physx::PxConvexMeshCookingResult::ePOLYGONS_LIMIT_REACHED:
            LOGE("cook convex mesh faild: limit reached");
            break;
        case physx::PxConvexMeshCookingResult::eFAILURE:
            LOGE("cook convex mesh faild: failure");
            break;
        case physx::PxConvexMeshCookingResult::eNON_GPU_COMPATIBLE:
            LOGE("cook convex mesh faild: non gpu compatible");
            break;
        default:;
    }

    physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
    return m_physics->createConvexMesh(input);
}

Shape ContextImpl::CreateShape(const SphereGeometry& geom,
                               const Material& material, bool is_exclusive) {
    physx::PxShape* shape = m_physics->createShape(
        Geometry2PhysX(geom), *material.GetImpl()->m_mtl, is_exclusive);
    if (shape) {
        return Shape{m_shape_allocator.Allocate(this, shape)};
    }
    return {};
}

Shape ContextImpl::CreateShape(const BoxGeometry& geom,
                               const Material& material, bool is_exclusive) {
    physx::PxShape* shape = m_physics->createShape(
        Geometry2PhysX(geom), *material.GetImpl()->m_mtl, is_exclusive);
    if (shape) {
        return Shape{m_shape_allocator.Allocate(this, shape)};
    }
    return {};
}

Shape ContextImpl::CreateShape(const CapsuleGeometry& geom,
                               const Material& material, bool is_exclusive) {
    physx::PxShape* shape = m_physics->createShape(
        Geometry2PhysX(geom), *material.GetImpl()->m_mtl, is_exclusive);
    if (shape) {
        return Shape{m_shape_allocator.Allocate(this, shape)};
    }
    return {};
}

Shape ContextImpl::CreateShape(const TriangleMeshGeometry& geom,
                               const Material& material, bool is_exclusive) {
    physx::PxShape* shape = m_physics->createShape(
        Geometry2PhysX(geom, geom.m_rotation, geom.m_scale),
        *material.GetImpl()->m_mtl, is_exclusive);
    if (shape) {
        return Shape{m_shape_allocator.Allocate(this, shape)};
    }
    return {};
}

Shape ContextImpl::CreateShape(const ConvexMeshGeometry& geom,
                               const Material& material, bool is_exclusive) {
    physx::PxShape* shape = m_physics->createShape(
        Geometry2PhysX(geom, geom.m_rotation, geom.m_scale),
        *material.GetImpl()->m_mtl, is_exclusive);
    if (shape) {
        return Shape{m_shape_allocator.Allocate(this, shape)};
    }
    return {};
}

Shape ContextImpl::CreateShape(const PlaneGeometry& geom,
                               const Material& material, bool is_exclusive) {
    physx::PxShape* shape = m_physics->createShape(
        Geometry2PhysX(geom), *material.GetImpl()->m_mtl, is_exclusive);
    if (shape) {
        return Shape{m_shape_allocator.Allocate(this, shape)};
    }
    return {};
}

D6Joint ContextImpl::CreateD6Joint(const RigidActor& actor0, const Vec3& p0,
                                   const Quat& q0, const RigidActor& actor1,
                                   const Vec3& p1, const Quat& q1) {
    physx::PxD6Joint* joint =
        PxD6JointCreate(*m_physics, actor0.GetImpl()->m_actor,
                        physx::PxTransform{Vec3ToPhysX(p0), QuatToPhysX(q0)},
                        actor1.GetImpl()->m_actor,
                        physx::PxTransform{Vec3ToPhysX(p1), QuatToPhysX(q1)});
    if (joint) {
        return m_joint_allocator.Allocate(this, joint);
    }
    return {};
}

void ContextImpl::Update(float delta_time) {
    m_vehicle_manager->Update(delta_time);
    m_main_scene.Simulate(delta_time);
}

Scene ContextImpl::GetMainScene() {
    return m_main_scene;
}

VehicleManager& ContextImpl::GetVehicleManager() {
    return *m_vehicle_manager;
}

const VehicleManager& ContextImpl::GetVehicleManager() const {
    return *m_vehicle_manager;
}

void ContextImpl::GC() {
    m_shape_allocator.GC();
    m_shape_const_allocator.GC();
    m_material_allocator.GC();
    m_joint_allocator.GC();
    m_rigid_actor_allocator.GC();
    m_rigid_actor_const_allocator.GC();
    m_vehicle_manager->GC();
    m_scene_allocator.GC();
}

}  // namespace nickel::physics