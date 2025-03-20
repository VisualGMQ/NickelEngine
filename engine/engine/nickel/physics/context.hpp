#pragma once
#include "nickel/common/math/math.hpp"
#include "nickel/physics/material.hpp"
#include "nickel/physics/rigidbody.hpp"
#include "nickel/physics/scene.hpp"
#include "nickel/physics/vehicle.hpp"

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

    TriangleMesh CreateTriangleMesh(std::span<const Vec3> vertices,
                                    std::span<const uint32_t> indices);
    
    ConvexMesh CreateConvexMesh(std::span<const Vec3> vertices);
    Shape CreateShape(const SphereGeometry&, const Material&,
                      bool is_exclusive = false);
    Shape CreateShape(const BoxGeometry&, const Material&,
                      bool is_exclusive = false);
    Shape CreateShape(const CapsuleGeometry&, const Material&,
                      bool is_exclusive = false);
    Shape CreateShape(const TriangleMeshGeometry&, const Material&,
                      bool is_exclusive = false);
    Shape CreateShape(const ConvexMeshGeometry&, const Material&,
                      bool is_exclusive = false);
    Shape CreateShape(const PlaneGeometry&, const Material&,
                      bool is_exclusive = false);

    VehicleManager& GetVehicleManager();
    const VehicleManager& GetVehicleManager() const;

    Scene GetMainScene();

    void Update(float delta_time);
    void GC();

    const ContextImpl* GetImpl() const;
    ContextImpl* GetImpl();

private:
    std::unique_ptr<ContextImpl> m_impl;
};

}  // namespace nickel::physics