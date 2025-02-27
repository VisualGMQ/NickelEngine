#pragma once
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/common/transform.hpp"
#include "nickel/graphics/gltf.hpp"
#include "nickel/graphics/mesh.hpp"
#include "nickel/physics/rigidbody.hpp"

namespace nickel {

class GameObject: public RefCountable {
public:
    std::string m_name;
    Transform m_transform;

    physics::RigidActor m_rigid_actor;
    graphics::Mesh m_model;

private:
    Transform m_global_transform;
    std::vector<GameObject*> m_children;
};

}