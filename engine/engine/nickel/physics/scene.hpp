#pragma once
#include "nickel/physics/cct.hpp"
#include "nickel/common/flags.hpp"
#include "nickel/physics/enums.hpp"
#include "nickel/physics/geom_query.hpp"
#include "nickel/physics/rigidbody.hpp"

namespace nickel::physics {

class ContextImpl;
class SceneImpl;

struct FilterData {
    uint32_t m_word0{}, m_word1{}, m_word2{}, m_word3{};
};

struct QueryFilterData {
    FilterData m_filter;
    Flags<QueryFlag> m_flags;
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

class Scene: public ImplWrapper<SceneImpl> {
public:
    using ImplWrapper::ImplWrapper;
    
    void AddRigidActor(RigidActor&);
    void Simulate(float delta_time) const;

    bool Raycast(const Vec3& origin, const Vec3& unit_dir, float distance,
                 RaycastHitCallback& hit_callback,
                 const QueryFilterData& filter_data,
                 Flags<HitFlag> hit_flags = HitFlag::Default,
                 QueryFilterCallback* filter_callback = nullptr);
    bool Sweep(const Geometry& geometry, const Vec3& p, const Quat& q,
               const Vec3& unit_dir, float distance,
               SweepHitCallback& hit_callback,
               const QueryFilterData& filter_data,
               Flags<HitFlag> hit_flags = HitFlag::Default,
               QueryFilterCallback* filter_callback = nullptr,
               float inflation = 0.0f);
    bool Overlap(const Geometry& geometry, const Vec3& p, const Quat& q,
                 OverlapHitCallback& hit_callback,
                 const QueryFilterData& filter_data,
                 QueryFilterCallback* filter_callback = nullptr);
    
    void EnableCCTOverlapRecoveryModule(bool enable);
    CapsuleController CreateCapsuleController(const CapsuleController::Descriptor&);

    void GC();
};

}  // namespace nickel::physics