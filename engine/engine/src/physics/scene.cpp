#include "nickel/physics/scene.hpp"
#include "nickel/physics/internal/scene_impl.hpp"

namespace nickel::physics {

Scene::Scene(SceneImpl* impl) : m_impl{impl} {}

Scene::Scene(const Scene& o) : m_impl{o.m_impl} {
    m_impl->IncRefcount();
}

Scene::Scene(Scene&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

Scene& Scene::operator=(const Scene& o) {
    if (&o != this) {
        m_impl->DecRefcount();
        m_impl = o.m_impl;
        m_impl->IncRefcount();
    }
    return *this;
}

Scene& Scene::operator=(Scene&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

Scene::~Scene() {
    if (m_impl) {
        m_impl->DecRefcount();
    }
}

void Scene::Simulate(float delta_time) const {
    m_impl->Simulate(delta_time);
}

Scene::operator bool() const {
    return m_impl;
}

const SceneImpl* Scene::GetImpl() const {
    return m_impl;
}

SceneImpl* Scene::GetImpl() {
    return m_impl;
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