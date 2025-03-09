#pragma once
#include "nickel/common/flags.hpp"
#include "nickel/common/math/math.hpp"
#include "nickel/physics/enums.hpp"
#include "nickel/physics/rigidbody.hpp"

namespace nickel::physics {

struct FilterData {
    uint32_t m_word0{}, m_word1{}, m_word2{}, m_word3{};
};

struct QueryFilterData {
    FilterData m_filter;
    Flags<QueryFlag> m_flags;
};

struct BasicHitInfo {
    Flags<HitFlag> m_flags;
    Vec3 m_position;
    Vec3 m_normal;
    float m_distance{};
    uint32_t m_face_index{};

    bool HadInitialOverlap() const { return m_distance <= 0.0f; }
};

struct Bound {
    Vec3 m_min, m_max;
};

struct GeomRaycastHit : public BasicHitInfo {
    float m_u{}, m_v{};
};

struct GeomSweepHit : public BasicHitInfo {};

struct GeomOverlapHit {
    uint32_t m_face_index{};
};

struct RaycastHit : public GeomRaycastHit {
    RigidActor m_actor;
    Shape m_shape;
};

struct SweepHit : public GeomSweepHit {
    RigidActor m_actor;
    Shape m_shape;
};

struct OverlapHit : public GeomOverlapHit {
    RigidActor m_actor;
    Shape m_shape;
};

template <typename HitType>
struct HitCallback {
    HitType block;
    bool hasBlock{};
    std::span<HitType> touches;
    uint32_t nbTouches{};

    HitCallback(std::span<HitType> buffer) : touches{buffer} {}

    virtual bool processTouches(std::span<HitType> buffer) = 0;
    virtual ~HitCallback() = default;
};

template <typename HitType>
struct QueryHitCallback : public HitCallback<HitType> {
    QueryHitCallback(std::span<HitType> hits)
        : HitCallback<HitType>(&hits.front(), hits.size()) {}

private:
    std::span<HitType> m_hits;
};

using RaycastHitCallback = QueryHitCallback<RaycastHit>;
using SweepHitCallback = QueryHitCallback<SweepHit>;
using OverlapHitCallback = QueryHitCallback<OverlapHit>;

struct QueryFilterCallback {
    virtual ~QueryFilterCallback() = default;
    virtual QueryHitType PreFilter(const FilterData&, const ShapeConst&,
                                   const RigidActorConst&, Flags<HitFlag>) = 0;
    virtual QueryHitType PostFilter(const FilterData&, const ShapeConst&,
                                    const RigidActorConst&) = 0;
};

}  // namespace nickel::physics