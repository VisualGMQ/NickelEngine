#pragma once
#include "nickel/common/math/math.hpp"
#include "nickel/physics/material.hpp"
#include "nickel/physics/rigidbody.hpp"
#include "nickel/physics/scene.hpp"

namespace nickel::physics {

class ContextImpl;

class Context {
public:
    Context();
    ~Context();

    Scene CreateScene(const std::string& name, const Vec3& gravity);
    Material CreateMaterial(float static_friction, float dynamic_friction,
                            float restitution);
    RigidStatic CreateRigidStatic(const Vec3& p, const Quat& q);
    RigidDynamic CreateRigidDynamic(const Vec3& p, const Quat& q);

    TriangleMesh CreateTriangleMesh(const Vec3* vertices, uint32_t vertex_count,
                                    const uint32_t* indices,
                                    uint32_t index_count);
    Shape CreateShape(const SphereGeometry&, const Material&);
    Shape CreateShape(const BoxGeometry&, const Material&);
    Shape CreateShape(const CapsuleGeometry&, const Material&);
    Shape CreateShape(const TriangleMeshGeometry&, const Material&);
    Shape CreateShape(const PlaneGeometry&, const Material&);

    void Update(float delta_time);
    void GC();

    const ContextImpl* GetImpl() const;
    ContextImpl* GetImpl();

private:
    std::unique_ptr<ContextImpl> m_impl;
};

}  // namespace nickel::physics