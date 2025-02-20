#pragma once
#include "nickel/common/math/math.hpp"

// fwd
namespace physx {
class PxTriangleMesh;
}

namespace nickel::physics {

struct Geometry {
    enum class Type {
        Box,
        Sphere,
        Capsule,
        TriangleMesh,
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
    
    TriangleMeshGeometry();
    explicit TriangleMeshGeometry(const TriangleMesh& mesh);
};

}  // namespace nickel::physics