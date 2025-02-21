#include "nickel/physics/geom_query.hpp"

#include "nickel/physics/internal/enum_convert.hpp"
#include "nickel/physics/internal/geometry_converter.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/internal/util.hpp"

namespace nickel::physics {

GeomRaycastHit GeomRaycastHitFromPhysX(const physx::PxGeomRaycastHit& hit) {
    GeomRaycastHit result;
    result.m_u = hit.u;
    result.m_v = hit.v;
    result.m_distance = hit.distance;
    result.m_normal = Vec3FromPhysX(hit.normal);
    result.m_position = Vec3FromPhysX(hit.position);
    result.m_flags = HitFlagFromPhysX(hit.flags);
    return result;
}

GeomSweepHit GeomSweepHitFromPhysX(const physx::PxGeomSweepHit& hit) {
    GeomSweepHit result;
    result.m_distance = hit.distance;
    result.m_normal = Vec3FromPhysX(hit.normal);
    result.m_position = Vec3FromPhysX(hit.position);
    result.m_flags = HitFlagFromPhysX(hit.flags);
    return result;
}

uint32_t GeometryQuery::Raycast(const Vec3& origin, const Vec3& unit_dir,
                                const Geometry& geom, const Vec3& p,
                                const Quat& q, float max_dist,
                                Flags<HitFlag> hit_flags,
                                std::vector<GeomRaycastHit>& hit_results) {
    std::vector<physx::PxRaycastHit> hits;
    hits.resize(hit_results.size());
    uint32_t hit_num = physx::PxGeometryQuery::raycast(
        Vec3ToPhysX(origin), Vec3ToPhysX(unit_dir), Geometry2PhysX(geom).any(),
        {Vec3ToPhysX(p), QuatToPhysX(q)}, max_dist, HitFlag2PhysX(hit_flags),
        hits.size(), hits.data());

    for (uint32_t i = 0; i < hit_num; i++) {
        hit_results[i] = GeomRaycastHitFromPhysX(hits[i]);
    }

    return hit_num;
}

bool GeometryQuery::ComputePenetration(Vec3& direction, float& depth,
                                       const Geometry& geom0, const Vec3& p0,
                                       const Quat& q0, const Geometry& geom1,
                                       const Vec3& p1, const Quat& q1) {
    physx::PxVec3 dir;
    bool is_penetrating = physx::PxGeometryQuery::computePenetration(
        dir, depth, Geometry2PhysX(geom0).any(),
        {Vec3ToPhysX(p0), QuatToPhysX(q0)}, Geometry2PhysX(geom1).any(),
        {Vec3ToPhysX(p1), QuatToPhysX(q1)});
    direction = Vec3FromPhysX(dir);
    return is_penetrating;
}

float GeometryQuery::PointDistance(const Vec3& point, const Geometry& geom,
                                   const Vec3& p, const Quat& q,
                                   Vec3* closest_point,
                                   uint32_t* closest_triangle_index) {
    physx::PxVec3 result_point;
    float dist = physx::PxGeometryQuery::pointDistance(
        Vec3ToPhysX(p), Geometry2PhysX(geom).any(),
        {Vec3ToPhysX(p), QuatToPhysX(q)}, &result_point,
        closest_triangle_index);

    if (closest_point) {
        *closest_point = Vec3FromPhysX(result_point);
    }
    return dist;
}

Bound GeometryQuery::ComputeGeomBounds(const Geometry& geom, const Vec3& p,
                                       const Quat& q, float offset,
                                       float inflation) {
    physx::PxBounds3 bound;
    physx::PxGeometryQuery::computeGeomBounds(bound, Geometry2PhysX(geom).any(),
                                              {Vec3ToPhysX(p), QuatToPhysX(q)},
                                              offset, inflation);
    Bound result;
    result.m_min = Vec3FromPhysX(bound.minimum);
    result.m_max = Vec3FromPhysX(bound.maximum);
    return result;
}

bool GeometryQuery::Overlap(const Geometry& geom0, const Vec3& p0,
                            const Quat& q0, const Geometry& geom1,
                            const Vec3& p1, const Quat& q1) {
    return physx::PxGeometryQuery::overlap(
        Geometry2PhysX(geom0).any(), {Vec3ToPhysX(p0), QuatToPhysX(q0)},
        Geometry2PhysX(geom1).any(), {Vec3ToPhysX(p1), QuatToPhysX(q1)});
}

uint32_t GeometryQuery::Sweep(const Vec3& unit_dir, float max_dist,
                              const Geometry& geom_src, const Vec3& p_src,
                              const Quat& q_src, const Geometry& geom_dst,
                              const Vec3& p_dst, const Quat& q_dst,
                              GeomSweepHit& hit_result,
                              Flags<HitFlag> hit_flags) {
    physx::PxGeomSweepHit hit;
    uint32_t hit_num = physx::PxGeometryQuery::sweep(
        Vec3ToPhysX(unit_dir), max_dist, Geometry2PhysX(geom_src).any(),
        {Vec3ToPhysX(p_src), QuatToPhysX(q_src)},
        Geometry2PhysX(geom_dst).any(),
        {Vec3ToPhysX(p_dst), QuatToPhysX(q_dst)}, hit,
        HitFlag2PhysX(hit_flags));
    hit_result = GeomSweepHitFromPhysX(hit);
    return hit_num;
}

}  // namespace nickel::physics