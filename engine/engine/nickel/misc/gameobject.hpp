#pragma once
#include "nickel/animation/skeleton.hpp"
#include "nickel/common/transform.hpp"
#include "nickel/graphics/gltf.hpp"
#include "nickel/physics/cct.hpp"
#include "nickel/physics/rigidbody.hpp"
#include "nickel/physics/vehicle.hpp"

namespace nickel {

class GameObject {
public:
    friend class Level;

    GameObject() = default;
    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;
    GameObject(GameObject&&) noexcept = default;
    GameObject& operator=(GameObject&&) noexcept = default;

    std::string m_name;
    Transform m_transform;

    physics::RigidActor m_rigid_actor;
    graphics::GLTFModel m_model;
    Skeleton m_skeleton;
    physics::CapsuleController m_controller;
    physics::Vehicle m_vehicle;

    // temporary for debug
    bool m_enable_render_model = true;

    const Transform& GetGlobalTransform() const { return m_transform; }

    void UpdateGlobalTransform(const Transform& parent_transform) {
        m_global_transform = parent_transform * m_transform;
    }

    std::vector<GameObject> m_children;

private:
    Transform m_global_transform;
};

}  // namespace nickel