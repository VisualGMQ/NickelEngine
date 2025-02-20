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

}  // namespace nickel::physics