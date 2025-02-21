#pragma once
#include "context.hpp"
#include "geometry.hpp"
#include "nickel/common/math/math.hpp"

namespace nickel::physics {

struct BasicHitInfo {
    Flags<HitFlag> m_flags;
    Vec3 m_position;
    Vec3 m_normal;
    float m_distance;

    bool HadInitialOverlap() const { return m_distance <= 0.0f; }
};

struct Bound {
    Vec3 m_min, m_max;
};

struct GeomRaycastHit : public BasicHitInfo {
    float m_u{}, m_v{};
};

struct GeomSweepHit : public BasicHitInfo {};

class GeometryQuery {
public:
    static uint32_t Raycast(const Vec3& origin, const Vec3& unit_dir,
                            const Geometry& geom, const Vec3& p, const Quat& q,
                            float max_dist, Flags<HitFlag> hit_flags,
                            std::vector<GeomRaycastHit>& hit_results);

    static bool Overlap(const Geometry& geom0, const Vec3& p0, const Quat& q0,
                        const Geometry& geom1, const Vec3& p1, const Quat& q1);

    static uint32_t Sweep(const Vec3& unit_dir, float max_dist,
                          const Geometry& geom_src, const Vec3& p_src,
                          const Quat& q_src, const Geometry& geom_dst,
                          const Vec3& p_dst, const Quat& q_dst,
                          GeomSweepHit& hit_result, Flags<HitFlag> hit_flags);

    static bool ComputePenetration(Vec3& unit_dir, float& depth,
                                   const Geometry& geom0, const Vec3& p0,
                                   const Quat& q0, const Geometry& geom1,
                                   const Vec3& p1, const Quat& q1);

    static float PointDistance(const Vec3& point, const Geometry& geom,
                               const Vec3& p, const Quat& q,
                               Vec3* closest_point = nullptr,
                               uint32_t* closest_triangle_index = nullptr);

    static Bound ComputeGeomBounds(const Geometry& geom, const Vec3& p,
                                   const Quat& q, float offset = 0,
                                   float inflation = 1);
};

}  // namespace nickel::physics