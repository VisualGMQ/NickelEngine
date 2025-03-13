#pragma once
#include "nickel/physics/geometry.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "util.hpp"

namespace nickel::physics {

inline physx::PxSphereGeometry Geometry2PhysX(const SphereGeometry& sphere) {
    return physx::PxSphereGeometry{sphere.m_radius};
}

inline physx::PxBoxGeometry Geometry2PhysX(const BoxGeometry& box) {
    return physx::PxBoxGeometry{box.m_half_extents.w,
                                box.m_half_extents.h,
                                box.m_half_extents.l};
}

inline physx::PxCapsuleGeometry Geometry2PhysX(const CapsuleGeometry& c) {
    return physx::PxCapsuleGeometry{c.m_radius, c.m_half_height};
}

inline physx::PxPlaneGeometry Geometry2PhysX(const PlaneGeometry&) {
    return physx::PxPlaneGeometry{};
}


inline physx::PxTriangleMeshGeometry Geometry2PhysX(
    const TriangleMeshGeometry& mesh, const Quat& rotation, const Vec3& scale) {
    physx::PxMeshScale scaling{Vec3ToPhysX(scale), QuatToPhysX(rotation)};
    return physx::PxTriangleMeshGeometry{mesh.m_data.m_mesh, scaling};
}

inline physx::PxConvexMeshGeometry Geometry2PhysX(const ConvexMeshGeometry& g,
                                                  const Quat& rotation,
                                                  const Vec3& scale) {
    physx::PxMeshScale scaling{Vec3ToPhysX(scale), QuatToPhysX(rotation)};
    return physx::PxConvexMeshGeometry{g.m_data.m_mesh, scaling};
}

// NOTE: only triangle mesh & convex mesh geometry can use rotation & scale
inline physx::PxGeometryHolder Geometry2PhysX(const Geometry& g,
                                              const Quat& rotation = {},
                                              const Vec3& scale = {1, 1, 1}) {
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
            holder.storeAny(Geometry2PhysX(
                static_cast<const TriangleMeshGeometry&>(g), rotation, scale));
            break;
        case Geometry::Type::Plane:
            holder.storeAny(
                Geometry2PhysX(static_cast<const PlaneGeometry&>(g)));
            break;
        case Geometry::Type::Convex:
            holder.storeAny(Geometry2PhysX(
                static_cast<const ConvexMeshGeometry&>(g), rotation, scale));
            break;
    }
    return holder;
}

}  // namespace nickel::physics