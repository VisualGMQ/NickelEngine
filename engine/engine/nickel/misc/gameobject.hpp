#pragma once
#include "nickel/common/transform.hpp"
#include "nickel/graphics/gltf.hpp"
#include "nickel/physics/cct.hpp"
#include "nickel/physics/rigidbody.hpp"

namespace nickel {

class GameObject {
public:
    friend class Level;

    std::string m_name;
    Transform m_transform;

    physics::RigidActor m_rigid_actor;
    graphics::GLTFModel m_model;
    physics::CapsuleController m_controller;

    const Transform& GetGlobalTransform() const { return m_transform; }

    void UpdateGlobalTransform(const Transform& parent_transform) {
        m_global_transform = parent_transform * m_transform;
    }

    std::vector<GameObject> m_children;

private:
    Transform m_global_transform;
};

}  // namespace nickel