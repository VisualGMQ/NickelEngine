#pragma once
#include "nickel/physics/cct.hpp"
#include "nickel/common/flags.hpp"
#include "nickel/physics/enums.hpp"
#include "nickel/physics/geom_query.hpp"
#include "nickel/physics/rigidbody.hpp"
#include "nickel/physics/filter.hpp"

namespace nickel::physics {

class ContextImpl;
class SceneImpl;

class Scene: public ImplWrapper<SceneImpl> {
public:
    using ImplWrapper::ImplWrapper;
    
    void AddRigidActor(RigidActor&);
    void Simulate(float delta_time) const;

    bool Raycast(const Vec3& origin, const Vec3& unit_dir, float distance,
                 RaycastHitCallback& hit_callback,
                 const QueryFilterData& filter_data,
                 Flags<HitFlag> hit_flags = HitFlag::Default);
    bool Sweep(const Geometry& geometry, const Vec3& p, const Quat& q,
               const Vec3& unit_dir, float distance,
               SweepHitCallback& hit_callback,
               const QueryFilterData& filter_data,
               Flags<HitFlag> hit_flags = HitFlag::Default,
               float inflation = 0.0f);
    bool Overlap(const Geometry& geometry, const Vec3& p, const Quat& q,
                 OverlapHitCallback& hit_callback,
                 const QueryFilterData& filter_data);

    void EnableCCTOverlapRecoveryModule(bool enable);
    CapsuleController CreateCapsuleController(
        const CapsuleController::Descriptor&);

    void GC();
};

}  // namespace nickel::physics