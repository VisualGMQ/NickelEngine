#include "nickel/physics/scene.hpp"
#include "nickel/physics/internal/scene_impl.hpp"

namespace nickel::physics {

void Scene::AddRigidActor(RigidActor& actor) {
    return m_impl->AddRigidActor(actor);
}

void Scene::Simulate(float delta_time) const {
    m_impl->Simulate(delta_time);
}

bool Scene::Raycast(const Vec3& origin, const Vec3& unit_dir, float distance,
                    RaycastHitCallback& hit_callback,
                    const QueryFilterData& filter_data,
                    Flags<HitFlag> hit_flags) {
    return m_impl->Raycast(origin, unit_dir, distance, hit_callback,
                           filter_data, hit_flags);
}

bool Scene::Sweep(const Geometry& geometry, const Vec3& p, const Quat& q,
                  const Vec3& unit_dir, float distance,
                  SweepHitCallback& hit_callback,
                  const QueryFilterData& filter_data, Flags<HitFlag> hit_flags,
                  float inflation) {
    return m_impl->Sweep(geometry, p, q, unit_dir, distance, hit_callback,
                         filter_data, hit_flags, inflation);
}

bool Scene::Overlap(const Geometry& geometry, const Vec3& p, const Quat& q,
                    OverlapHitCallback& hit_callback,
                    const QueryFilterData& filter_data) {
    return m_impl->Overlap(geometry, p, q, hit_callback, filter_data);
}

void Scene::EnableCCTOverlapRecoveryModule(bool enable) {
    m_impl->EnableCCTOverlapRecoveryModule(enable);
}

CapsuleController Scene::CreateCapsuleController(
    const CapsuleController::Descriptor& desc) {
    return m_impl->CreateCapsuleController(desc);
}

void Scene::GC() {
    m_impl->GC();
}

}  // namespace nickel::physics