#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/physics/internal/cct_impl.hpp"
#include "nickel/physics/internal/enum_convert.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/internal/rigidbody_impl.hpp"
#include "nickel/physics/internal/shape_impl.hpp"
#include "nickel/physics/rigidbody.hpp"
#include "nickel/physics/scene.hpp"

namespace nickel::physics {

class ContextImpl;

template <typename HitType, typename PhysXHitType>
PhysXHitType HitType2PhysX(const HitType& hit);

template <>
physx::PxRaycastHit HitType2PhysX(const RaycastHit& hit);
template <>
physx::PxSweepHit HitType2PhysX(const SweepHit& hit);
template <>
physx::PxOverlapHit HitType2PhysX(const OverlapHit& hit);

template <typename HitType, typename PhysXHitType>
HitType HitTypeFromPhysX(ContextImpl& ctx, const PhysXHitType& hit);

template <>
RaycastHit HitTypeFromPhysX(ContextImpl& ctx, const physx::PxRaycastHit& hit);
template <>
SweepHit HitTypeFromPhysX(ContextImpl& ctx, const physx::PxSweepHit& hit);
template <>
OverlapHit HitTypeFromPhysX(ContextImpl& ctx, const physx::PxOverlapHit& hit);

physx::PxFilterData FilterData2PhysX(const FilterData& data);

FilterData FilterDataFromPhysX(const physx::PxFilterData& data);

inline physx::PxQueryFilterData QueryFilterData2PhysX(
    const QueryFilterData& data) {
    physx::PxQueryFilterData result;
    result.data.word0 = data.m_filter.m_word0;
    result.data.word1 = data.m_filter.m_word1;
    result.data.word2 = data.m_filter.m_word2;
    result.data.word3 = data.m_filter.m_word3;
    result.flags = QueryFlags2PhysX(data.m_flags);
    return result;
}

template <typename HitType, typename PhysXHitType>
struct PhysicsQueryCallback : public physx::PxHitCallback<PhysXHitType> {
    PhysicsQueryCallback(ContextImpl& ctx, QueryHitCallback<HitType>& callback)
        : physx::PxHitCallback<PhysXHitType>(nullptr, 0),
          m_callback{callback},
          m_ctx{ctx} {
        m_hits.resize(callback.nbTouches);
        this->touches = m_hits.data();
        this->maxNbTouches = m_hits.size();
    }

    physx::PxAgain processTouches(const PhysXHitType* buffer,
                                  physx::PxU32 nbHits) override {
        // TODO: std::vector memory allocate too frequency!
        std::vector<HitType> hits;
        hits.reserve(nbHits);
        for (uint32_t i = 0; i < nbHits; i++) {
            hits.push_back(
                HitTypeFromPhysX<HitType, PhysXHitType>(m_ctx, buffer[i]));
        }
        return m_callback.processTouches(std::span{hits});
    }

private:
    std::vector<PhysXHitType> m_hits;
    QueryHitCallback<HitType>& m_callback;
    ContextImpl& m_ctx;
};

using PhysicsRaycastCallback =
    PhysicsQueryCallback<RaycastHit, physx::PxRaycastHit>;
using PhysicsSweepCallback = PhysicsQueryCallback<SweepHit, physx::PxSweepHit>;
using PhysicsOverlapCallback =
    PhysicsQueryCallback<OverlapHit, physx::PxOverlapHit>;

class PhysXQueryFilterCallback : public physx::PxQueryFilterCallback {
public:
    PhysXQueryFilterCallback(ContextImpl& ctx, QueryFilterCallback* callback);

    physx::PxQueryHitType::Enum preFilter(
        const physx::PxFilterData& filterData, const physx::PxShape* shape,
        const physx::PxRigidActor* actor,
        physx::PxHitFlags& query_flags) override;

    physx::PxQueryHitType::Enum postFilter(
        const physx::PxFilterData& filterData, const physx::PxQueryHit& hit,
        const physx::PxShape* shape, const physx::PxRigidActor* actor) override;

private:
    QueryFilterCallback* m_callback{};
    ContextImpl& m_ctx;
};

class SceneImpl : public RefCountable {
public:
    SceneImpl(const std::string& name, ContextImpl* ctx, physx::PxScene*);
    ~SceneImpl();

    void DecRefcount() override;

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
    void GC();

    CapsuleController CreateCapsuleController(const CapsuleController::Descriptor&);

    physx::PxScene* m_scene{};
    physx::PxControllerManager* m_cct_manager{};
    BlockMemoryAllocator<CapsuleControllerImpl> m_capsule_controller_allocator;
    BlockMemoryAllocator<CapsuleControllerConstImpl> m_const_capsule_controller_allocator;

private:
    ContextImpl* m_ctx;
};

}  // namespace nickel::physics