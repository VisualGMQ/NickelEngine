#pragma once
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/geometry.hpp"

namespace nickel::physics {

inline physx::PxSphereGeometry Geometry2PhysX(const SphereGeometry& sphere) {
    return physx::PxSphereGeometry{sphere.m_radius};
}

inline physx::PxBoxGeometry Geometry2PhysX(const BoxGeometry& box) {
    return physx::PxBoxGeometry{box.m_half_extents.w * 0.5f,
                                box.m_half_extents.h * 0.5f,
                                box.m_half_extents.l * 0.5f};
}

inline physx::PxCapsuleGeometry Geometry2PhysX(const CapsuleGeometry& c) {
    return physx::PxCapsuleGeometry{c.m_radius, c.m_half_height};
}

inline physx::PxPlaneGeometry Geometry2PhysX(const PlaneGeometry&) {
    return physx::PxPlaneGeometry{};
}

inline physx::PxTriangleMeshGeometry Geometry2PhysX(const TriangleMeshGeometry& mesh) {
    return physx::PxTriangleMeshGeometry{mesh.m_data.m_mesh};
}

inline physx::PxGeometryHolder Geometry2PhysX(const Geometry& g) {
    physx::PxGeometryHolder holder;
    switch (g.GetType()) {
        case Geometry::Type::Box:
            holder.storeAny(Geometry2PhysX(static_cast<const BoxGeometry&>(g)));
            break;
        case Geometry::Type::Sphere:
            holder.storeAny(
                Geometry2PhysX(static_cast<const SphereGeometry&>(g)));
            break;
        case Geometry::Type::Capsule:
            holder.storeAny(
                Geometry2PhysX(static_cast<const CapsuleGeometry&>(g)));
            break;
        case Geometry::Type::TriangleMesh:
            holder.storeAny(
                Geometry2PhysX(static_cast<const TriangleMeshGeometry&>(g)));
            break;
        case Geometry::Type::Plane:
            holder.storeAny(
                Geometry2PhysX(static_cast<const PlaneGeometry&>(g)));
            break;
    }
    return holder;
}

}  // namespace nickel::physics