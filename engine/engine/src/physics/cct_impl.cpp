#include "nickel/physics/internal/cct_impl.hpp"

#include "nickel/common/log.hpp"
#include "nickel/physics/internal/material_impl.hpp"
#include "nickel/physics/internal/scene_impl.hpp"
#include "nickel/physics/internal/util.hpp"

namespace nickel::physics {

PhysXControllerFilterCallback::PhysXControllerFilterCallback(
    physx::PxControllerManager& mgr, ContextImpl& ctx, SceneImpl& scene)
    : m_mgr(mgr), m_ctx(ctx), m_scene(scene) {}

CapsuleControllerImpl::CapsuleControllerImpl(
    physx::PxControllerManager& mgr, ContextImpl& ctx, SceneImpl& scene,
    const CapsuleController::Descriptor& desc)
    : m_ctx{&ctx}, m_scene{&scene} {
    physx::PxCapsuleControllerDesc physx_desc;
    physx_desc.height = desc.m_height;
    physx_desc.radius = desc.m_radius;
    physx_desc.climbingMode = ClimbingMode2PhysX(desc.m_climbing_mode);
    physx_desc.density = desc.m_density;
    physx_desc.material =
        desc.m_material ? desc.m_material.GetImpl()->m_mtl : nullptr;
    physx_desc.position.x = desc.m_position.x;
    physx_desc.position.y = desc.m_position.y;
    physx_desc.position.z = desc.m_position.z;
    physx_desc.contactOffset = desc.m_contact_offset;
    physx_desc.scaleCoeff = desc.m_scale_coeff;
    physx_desc.slopeLimit = desc.m_slope_limit;
    physx_desc.stepOffset = desc.m_step_offset;
    physx_desc.upDirection = Vec3ToPhysX(desc.m_up_dir);
    physx_desc.invisibleWallHeight = desc.m_invisible_wall_height;
    physx_desc.maxJumpHeight = desc.m_max_jump_height;
    physx_desc.nonWalkableMode = NonWalkableMode2PhysX(desc.m_nonwalkable_mode);
    physx_desc.registerDeletionListener = desc.m_register_deleteion_listener;

    m_cct = static_cast<physx::PxCapsuleController*>(
        mgr.createController(physx_desc));

    if (!m_cct) {
        LOGE("capsule controller create failed");
    }
}

CapsuleControllerImpl::CapsuleControllerImpl(
    physx::PxControllerManager&, ContextImpl& ctx, SceneImpl& scene,
    physx::PxCapsuleController* controller)
    : m_ctx{&ctx}, m_scene{&scene}, m_cct{controller} {}

CapsuleControllerImpl::~CapsuleControllerImpl() {
    if (m_cct) {
        m_cct->release();
    }
}

void CapsuleControllerImpl::SetRadius(float radius) {
    m_cct->setRadius(radius);
}

float CapsuleControllerImpl::GetRadius() const {
    return m_cct->getRadius();
}

void CapsuleControllerImpl::SetHeight(float height) {
    m_cct->setHeight(height);
}

float CapsuleControllerImpl::GetHeight() const {
    return m_cct->getHeight();
}

void CapsuleControllerImpl::Resize(float height) {
    m_cct->resize(height);
}

Flags<CCTCollisionFlag> CapsuleControllerImpl::MoveAndSlide(
    const Vec3& disp, float min_dist, float elapsed_time,
    ControllerFilters* filter) {
    // TODO: explose filter as parameter
    physx::PxControllerFilters filters;
    PhysXControllerFilterCallback filter_callback;

    if (filter) {
        if (filter->m_filter_data) {
            auto filter_data = FilterData2PhysX(*filter->m_filter_data);
            filters.mFilterData = &filter_data;
        }
        filters.mFilterFlags = QueryFlags2PhysX(filter->m_query_flags);

        if (filter->m_filter_callback) {
            PhysXQueryFilterCallback callback{*m_ctx,
                                              filter->m_filter_callback};
            filters.mFilterCallback = &callback;
        }
        if (filter->m_cct_callback) {
            PhysXControllerFilterCallback callback;
            callback.m_callback = filter->m_cct_callback;
            filters.mCCTFilterCallback = &callback;
        }
    }
    return CCTCollisionFlagFromPhysX(
        m_cct->move(Vec3ToPhysX(disp), min_dist, elapsed_time, filters));
}

void CapsuleControllerImpl::SetPosition(const Vec3& p) {
    m_cct->setPosition(physx::PxExtendedVec3{p.x, p.y, p.z});
}

void CapsuleControllerImpl::SetFootPosition(const Vec3&) {
    m_cct->setFootPosition(physx::PxExtendedVec3{0.0f, 0.0f, 0.0f});
}

Vec3 CapsuleControllerImpl::GetPosition() const {
    auto p = m_cct->getPosition();
    return Vec3(p.x, p.y, p.z);
}

Vec3 CapsuleControllerImpl::GetFootPosition() const {
    auto p = m_cct->getFootPosition();
    return Vec3(p.x, p.y, p.z);
}

void CapsuleControllerImpl::SetStepOffset(float offset) {
    m_cct->setStepOffset(offset);
}

float CapsuleControllerImpl::GetStepOffset() const {
    return m_cct->getStepOffset();
}

void CapsuleControllerImpl::SetUpDirection(const Vec3& dir) {
    m_cct->setUpDirection(Vec3ToPhysX(dir));
}

Vec3 CapsuleControllerImpl::GetUpDirection() const {
    return Vec3FromPhysX(m_cct->getUpDirection());
}

CCTDesc::NonWalkableMode CapsuleControllerImpl::GetNonWalkableMode() const {
    return NonWalkableModeFromPhysX(m_cct->getNonWalkableMode());
}

void CapsuleControllerImpl::SetNonWalkableMode(CCTDesc::NonWalkableMode mode) {
    m_cct->setNonWalkableMode(NonWalkableMode2PhysX(mode));
}

CapsuleController::Descriptor::ClimbingMode
CapsuleControllerImpl::GetClimbingMode() const {
    return ClimbingModeFromPhysX(m_cct->getClimbingMode());
}

void CapsuleControllerImpl::SetClimbingMode(
    CapsuleController::Descriptor::ClimbingMode mode) {
    m_cct->setClimbingMode(ClimbingMode2PhysX(mode));
}

void CapsuleControllerImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_scene->m_capsule_controller_allocator.MarkAsGarbage(this);
    }
}

CapsuleControllerConstImpl::CapsuleControllerConstImpl(
    physx::PxControllerManager& mgr, ContextImpl& ctx, SceneImpl& scene,
    const CapsuleController::Descriptor& desc)
    : CapsuleControllerImpl(mgr, ctx, scene, desc) {}

CapsuleControllerConstImpl::CapsuleControllerConstImpl(
    physx::PxControllerManager& mgr, ContextImpl& ctx, SceneImpl& scene,
    physx::PxCapsuleController* controller)
    : CapsuleControllerImpl(mgr, ctx, scene, controller) {}

void CapsuleControllerConstImpl::DecRefcount() {
    CapsuleControllerImpl::DecRefcount();

    if (Refcount() == 0) {
        m_scene->m_const_capsule_controller_allocator.MarkAsGarbage(this);
    }
}
}  // namespace nickel::physics