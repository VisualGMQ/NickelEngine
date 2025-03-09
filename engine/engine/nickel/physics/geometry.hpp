#pragma once
#include "nickel/common/math/math.hpp"

// fwd
namespace physx {
class PxTriangleMesh;
class PxConvexMesh;
}

namespace nickel::physics {

struct Geometry {
    enum class Type {
        Box,
        Sphere,
        Capsule,
        TriangleMesh,
        Convex,
        Plane,
    };

    explicit Geometry(Type type);
    virtual ~Geometry() = default;

    Type GetType() const noexcept;

private:
    Type m_type;
};

struct SphereGeometry: public Geometry {
    float m_radius = 0;

    SphereGeometry();
    SphereGeometry(float radius);
};

struct BoxGeometry: public Geometry {
    Vec3 m_half_extents;

    BoxGeometry();
    BoxGeometry(const Vec3& half_extents);
};

struct CapsuleGeometry: public Geometry {
    float m_radius = 0;
    float m_half_height = 0;

    CapsuleGeometry();
    CapsuleGeometry(float radius, float half_height);
};

// a YoZ plane, forward +X
struct PlaneGeometry: public Geometry {
    PlaneGeometry();
};

struct TriangleMesh {
    TriangleMesh() = default;
    TriangleMesh(physx::PxTriangleMesh*);
    TriangleMesh(const TriangleMesh&);
    TriangleMesh(TriangleMesh&&) noexcept;
    TriangleMesh& operator=(const TriangleMesh&) noexcept;
    TriangleMesh& operator=(TriangleMesh&&) noexcept;
    ~TriangleMesh();

    physx::PxTriangleMesh* m_mesh{};
};

struct TriangleMeshGeometry: public Geometry {
    TriangleMesh m_data;
    Quat m_rotation;
    Vec3 m_scale;
    
    TriangleMeshGeometry();
    explicit TriangleMeshGeometry(const TriangleMesh& mesh, const Quat& = {}, const Vec3& = {1, 1, 1});
};

struct ConvexMesh {
    ConvexMesh() = default;
    ConvexMesh(physx::PxConvexMesh*);
    ConvexMesh(const ConvexMesh&);
    ConvexMesh(ConvexMesh&&) noexcept;
    ConvexMesh& operator=(const ConvexMesh&) noexcept;
    ConvexMesh& operator=(ConvexMesh&&) noexcept;
    ~ConvexMesh();
    
    physx::PxConvexMesh* m_mesh{};
};

struct ConvexMeshGeometry: public Geometry {
    ConvexMesh m_data;
    Quat m_rotation;
    Vec3 m_scale;
    
    ConvexMeshGeometry();
    explicit ConvexMeshGeometry(const ConvexMesh& mesh, const Quat& = {}, const Vec3& = {});
};

}  // namespace nickel::physics