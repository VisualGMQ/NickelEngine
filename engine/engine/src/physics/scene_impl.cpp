#include "nickel/physics/internal/scene_impl.hpp"

#include "nickel/physics/internal/context_impl.hpp"

namespace nickel::physics {

SceneImpl::SceneImpl(const std::string& name, ContextImpl* ctx,
                     physx::PxScene* scene)
    : m_scene{scene}, m_ctx{ctx} {
    scene->setName(name.c_str());
}

SceneImpl::~SceneImpl() {
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

}  // namespace nickel::physics