#include "nickel/physics/geometry.hpp"
#include "nickel/physics/internal/pch.hpp"

#include "GuConvexSupport.h"

namespace nickel::physics {

Geometry::Geometry(Type type) : m_type{type} {}

Geometry::Type Geometry::GetType() const noexcept {
    return m_type;
}

SphereGeometry::SphereGeometry() : Geometry{Type::Sphere} {}

SphereGeometry::SphereGeometry(float radius)
    : Geometry{Type::Sphere}, m_radius{radius} {}

BoxGeometry::BoxGeometry() : Geometry{Type::Box} {}

BoxGeometry::BoxGeometry(const Vec3& half_extents)
    : Geometry{Type::Box}, m_half_extents{half_extents} {}

CapsuleGeometry::CapsuleGeometry() : Geometry{Type::Capsule} {}

CapsuleGeometry::CapsuleGeometry(float radius, float half_height)
    : Geometry{Type::Capsule}, m_radius{radius}, m_half_height{half_height} {}

PlaneGeometry::PlaneGeometry() : Geometry{Type::Plane} {}


TriangleMesh::TriangleMesh(physx::PxTriangleMesh* mesh) : m_mesh{mesh} {}

TriangleMesh::TriangleMesh(const TriangleMesh& o) : m_mesh{o.m_mesh} {
    if (m_mesh) {
        m_mesh->acquireReference();
    }
}

TriangleMesh::TriangleMesh(TriangleMesh&& o) noexcept : m_mesh{o.m_mesh} {
    o.m_mesh = nullptr;
}

TriangleMesh& TriangleMesh::operator=(const TriangleMesh& o) noexcept {
    if (&o != this) {
        if (m_mesh) {
            m_mesh->release();
        }
        m_mesh = o.m_mesh;
        if (m_mesh) {
            m_mesh->acquireReference();
        }
    }
    return *this;
}

TriangleMesh& TriangleMesh::operator=(TriangleMesh&& o) noexcept {
    if (&o != this) {
        if (m_mesh) {
            m_mesh->release();
        }
        m_mesh = o.m_mesh;
        o.m_mesh = nullptr;
    }
    return *this;
}

TriangleMesh::~TriangleMesh() {
    if (m_mesh) {
        m_mesh->release();
    }
}

TriangleMeshGeometry::TriangleMeshGeometry() : Geometry{Type::TriangleMesh} {}

TriangleMeshGeometry::TriangleMeshGeometry(const TriangleMesh& mesh)
    : Geometry{Type::TriangleMesh}, m_data(mesh) {}

}  // namespace nickel::physics