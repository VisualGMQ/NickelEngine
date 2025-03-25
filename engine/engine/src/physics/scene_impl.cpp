#include "nickel/physics/internal/scene_impl.hpp"
#include "nickel/physics/internal/cct_impl.hpp"
#include "nickel/physics/internal/context_impl.hpp"
#include "nickel/physics/internal/enum_convert.hpp"
#include "nickel/physics/internal/geometry_converter.hpp"
#include "nickel/physics/internal/rigidbody_impl.hpp"
#include "nickel/physics/internal/shape_impl.hpp"
#include "nickel/physics/internal/util.hpp"

namespace nickel::physics {

template <>
physx::PxRaycastHit HitType2PhysX<RaycastHit, physx::PxRaycastHit>(
    const RaycastHit& hit) {
    physx::PxRaycastHit result;
    result.distance = hit.m_distance;
    result.normal = Vec3ToPhysX(hit.m_normal);
    result.position = Vec3ToPhysX(hit.m_position);
    result.flags = HitFlag2PhysX(hit.m_flags);
    result.u = hit.m_u;
    result.v = hit.m_v;
    result.faceIndex = hit.m_face_index;
    result.actor = hit.m_actor.GetImpl()->m_actor;
    result.shape = hit.m_shape.GetImpl()->m_shape;
    return result;
}

template <>
physx::PxSweepHit HitType2PhysX<SweepHit, physx::PxSweepHit>(
    const SweepHit& hit) {
    physx::PxSweepHit result;
    result.distance = hit.m_distance;
    result.normal = Vec3ToPhysX(hit.m_normal);
    result.position = Vec3ToPhysX(hit.m_position);
    result.flags = HitFlag2PhysX(hit.m_flags);
    result.faceIndex = hit.m_face_index;
    result.actor = hit.m_actor.GetImpl()->m_actor;
    result.shape = hit.m_shape.GetImpl()->m_shape;
    return result;
}

template <>
physx::PxOverlapHit HitType2PhysX<OverlapHit, physx::PxOverlapHit>(
    const OverlapHit& hit) {
    physx::PxOverlapHit result;
    result.faceIndex = hit.m_face_index;
    result.actor = hit.m_actor.GetImpl()->m_actor;
    result.shape = hit.m_shape.GetImpl()->m_shape;
    return result;
}

template <>
RaycastHit HitTypeFromPhysX<RaycastHit, physx::PxRaycastHit>(
    ContextImpl& ctx, const physx::PxRaycastHit& hit) {
    RaycastHit result;
    result.m_distance = hit.distance;
    result.m_face_index = hit.faceIndex;
    result.m_normal = Vec3FromPhysX(hit.normal);
    result.m_position = Vec3FromPhysX(hit.position);
    result.m_flags = HitFlagFromPhysX(hit.flags);
    result.m_u = hit.u;
    result.m_v = hit.v;
    result.m_actor = ctx.m_rigid_actor_allocator.Allocate(&ctx, hit.actor);
    result.m_shape = ctx.m_shape_allocator.Allocate(&ctx, hit.shape);
    return result;
}

template <>
SweepHit HitTypeFromPhysX<SweepHit, physx::PxSweepHit>(
    ContextImpl& ctx, const physx::PxSweepHit& hit) {
    SweepHit result;
    result.m_distance = hit.distance;
    result.m_face_index = hit.faceIndex;
    result.m_normal = Vec3FromPhysX(hit.normal);
    result.m_position = Vec3FromPhysX(hit.position);
    result.m_flags = HitFlagFromPhysX(hit.flags);
    result.m_actor = ctx.m_rigid_actor_allocator.Allocate(&ctx, hit.actor);
    result.m_shape = ctx.m_shape_allocator.Allocate(&ctx, hit.shape);
    return result;
}

template <>
OverlapHit HitTypeFromPhysX<OverlapHit, physx::PxOverlapHit>(
    ContextImpl& ctx, const physx::PxOverlapHit& hit) {
    OverlapHit result;
    result.m_face_index = hit.faceIndex;
    result.m_actor = ctx.m_rigid_actor_allocator.Allocate(&ctx, hit.actor);
    result.m_shape = ctx.m_shape_allocator.Allocate(&ctx, hit.shape);
    return result;
}

SceneImpl::SceneImpl(const std::string& name, ContextImpl* ctx,
                     physx::PxScene* scene)
    : m_scene{scene}, m_ctx{ctx} {
    scene->setName(name.c_str());
    scene->getScenePvdClient()->setScenePvdFlags(
        physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS |
        physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES |
        physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS);
    m_cct_manager = PxCreateControllerManager(*scene);
    if (!m_cct_manager) {
        LOGE("physics cct create failed");
    }
}

SceneImpl::~SceneImpl() {
    m_capsule_controller_allocator.FreeAll();

    if (m_cct_manager) {
        m_cct_manager->release();
    }
    if (m_scene) {
        m_scene->release();
    }
}

void SceneImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0 && m_ctx) {
        m_ctx->m_scene_allocator.MarkAsGarbage(this);
    }
}

void SceneImpl::AddRigidActor(RigidActor& actor) {
    m_scene->addActor(*actor.GetImpl()->m_actor);
}

void SceneImpl::Simulate(float delta_time) const {
    m_scene->simulate(delta_time);
    m_scene->fetchResults(true);
}

bool SceneImpl::Raycast(const Vec3& origin, const Vec3& unit_dir,
                        float distance, RaycastHitCallback& hit_callback,
                        const QueryFilterData& filter_data,
                        Flags<HitFlag> hit_flags) const {
    PhysicsRaycastCallback physx_hit_callback{*m_ctx, hit_callback};

    bool has_touch = m_scene->raycast(
        Vec3ToPhysX(origin), Vec3ToPhysX(unit_dir), distance,
        physx_hit_callback, HitFlag2PhysX(hit_flags),
        QueryFilterData2PhysX(filter_data), &m_ctx->m_query_filter_callback);

    hit_callback.hasBlock = physx_hit_callback.hasBlock;
    hit_callback.nbTouches = physx_hit_callback.nbTouches;
    hit_callback.block =
        HitTypeFromPhysX<RaycastHit>(*m_ctx, physx_hit_callback.block);
    for (size_t i = 0; i < physx_hit_callback.nbTouches; i++) {
        hit_callback.touches[i] =
            HitTypeFromPhysX<RaycastHit>(*m_ctx, physx_hit_callback.touches[i]);
    }

    return has_touch;
}

bool SceneImpl::Sweep(const Geometry& geometry, const Vec3& p, const Quat& q,
                      const Vec3& unit_dir, float distance,
                      SweepHitCallback& hit_callback,
                      const QueryFilterData& filter_data,
                      Flags<HitFlag> hit_flags, float inflation) const {
    PhysicsSweepCallback physx_hit_callback{*m_ctx, hit_callback};

    bool has_touch = m_scene->sweep(
        Geometry2PhysX(geometry).any(), {Vec3ToPhysX(p), QuatToPhysX(q)},
        Vec3ToPhysX(unit_dir), distance, physx_hit_callback,
        HitFlag2PhysX(hit_flags), QueryFilterData2PhysX(filter_data),
        &m_ctx->m_query_filter_callback, nullptr, inflation);

    hit_callback.hasBlock = physx_hit_callback.hasBlock;
    hit_callback.nbTouches = physx_hit_callback.nbTouches;
    hit_callback.block =
        HitTypeFromPhysX<SweepHit>(*m_ctx, physx_hit_callback.block);
    for (size_t i = 0; i < physx_hit_callback.nbTouches; i++) {
        hit_callback.touches[i] =
            HitTypeFromPhysX<SweepHit>(*m_ctx, physx_hit_callback.touches[i]);
    }

    return has_touch;
}

bool SceneImpl::Overlap(const Geometry& geometry, const Vec3& p, const Quat& q,
                        OverlapHitCallback& hit_callback,
                        const QueryFilterData& filter_data) const {
    PhysicsOverlapCallback physx_hit_callback{*m_ctx, hit_callback};

    bool has_touch = m_scene->overlap(
        Geometry2PhysX(geometry).any(), {Vec3ToPhysX(p), QuatToPhysX(q)},
        physx_hit_callback, QueryFilterData2PhysX(filter_data),
        &m_ctx->m_query_filter_callback);

    hit_callback.hasBlock = physx_hit_callback.hasBlock;
    hit_callback.nbTouches = physx_hit_callback.nbTouches;
    hit_callback.block =
        HitTypeFromPhysX<OverlapHit>(*m_ctx, physx_hit_callback.block);
    for (size_t i = 0; i < physx_hit_callback.nbTouches; i++) {
        hit_callback.touches[i] =
            HitTypeFromPhysX<OverlapHit>(*m_ctx, physx_hit_callback.touches[i]);
    }

    return has_touch;
}

void SceneImpl::EnableCCTOverlapRecoveryModule(bool enable) {
    m_cct_manager->setOverlapRecoveryModule(enable);
}

void SceneImpl::GC() {
    m_capsule_controller_allocator.GC();
}

CapsuleController SceneImpl::CreateCapsuleController(
    const CapsuleController::Descriptor& desc) {
    return m_capsule_controller_allocator.Allocate(*m_cct_manager, *m_ctx,
                                                   *this, desc);
}

}  // namespace nickel::physics