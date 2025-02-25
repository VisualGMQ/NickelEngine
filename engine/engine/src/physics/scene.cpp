#include "nickel/physics/scene.hpp"
#include "nickel/physics/internal/scene_impl.hpp"

namespace nickel::physics {

void Scene::Simulate(float delta_time) const {
    m_impl->Simulate(delta_time);
}

bool Scene::Raycast(const Vec3& origin, const Vec3& unit_dir, float distance,
                    RaycastHitCallback& hit_callback,
                    const QueryFilterData& filter_data,
                    Flags<HitFlag> hit_flags,
                    QueryFilterCallback* filter_callback) {
    return m_impl->Raycast(origin, unit_dir, distance, hit_callback,
                           filter_data, hit_flags, filter_callback);
}

bool Scene::Sweep(const Geometry& geometry, const Vec3& p, const Quat& q,
                  const Vec3& unit_dir, float distance,
                  SweepHitCallback& hit_callback,
                  const QueryFilterData& filter_data, Flags<HitFlag> hit_flags,
                  QueryFilterCallback* filter_callback, float inflation) {
    return m_impl->Sweep(geometry, p, q, unit_dir, distance, hit_callback,
                         filter_data, hit_flags, filter_callback, inflation);
}

bool Scene::Overlap(const Geometry& geometry, const Vec3& p, const Quat& q,
                    OverlapHitCallback& hit_callback,
                    const QueryFilterData& filter_data,
                    QueryFilterCallback* filter_callback) {
    return m_impl->Overlap(geometry, p, q, hit_callback, filter_data,
                           filter_callback);
}

}  // namespace nickel::physics