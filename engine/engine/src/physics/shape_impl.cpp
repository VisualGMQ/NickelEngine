#include "nickel/physics/internal/shape_impl.hpp"

#include "nickel/physics/internal/context_impl.hpp"
#include "nickel/physics/internal/geometry_converter.hpp"
#include "nickel/physics/internal/material_impl.hpp"
#include "nickel/physics/internal/util.hpp"

/**
 * simulate filter data struct:
 * word0 - collision group(5bits), collision behavior(8bits)
 * word1 - is enabled overlap(in bits)
 * word2 - is enabled block(in bits)
 * word3 - no use
 *
 * query filter data struct:
 * word0 - collision group(5bits)
 * word1 - no use
 * word2 - no use
 * word3 - no use
 */

namespace nickel::physics {

ShapeImpl::ShapeImpl(ContextImpl* ctx, physx::PxShape* shape)
    : m_ctx{ctx}, m_shape{shape} {}

void ShapeImpl::SetMaterials(std::span<Material> materials) {
    std::vector<physx::PxMaterial*> mtls;
    mtls.reserve(materials.size());
    for (auto m : materials) {
        mtls.push_back(m.GetImpl()->m_mtl);
    }

    m_shape->setMaterials(mtls.data(), mtls.size());
}

void ShapeImpl::SetMaterial(Material& mtl) {
    SetMaterials(std::span{&mtl, 1});
}

void ShapeImpl::SetGeometry(const Geometry& g) {
    auto holder = Geometry2PhysX(g);
    m_shape->setGeometry(holder.any());
}

void ShapeImpl::SetLocalPose(const Vec3& p, const Quat& q) {
    m_shape->setLocalPose({Vec3ToPhysX(p), QuatToPhysX(q)});
}

Transform ShapeImpl::GetLocalPose() const {
    return TransformFromPhysX(m_shape->getLocalPose());
}

void ShapeImpl::SetCollisionGroup(CollisionGroup group) {
    auto numeric = static_cast<std::underlying_type_t<CollisionGroup>>(group);
    auto simulate_filter_data = m_shape->getSimulationFilterData();
    simulate_filter_data.word0 = numeric;

    // default simulation behavior is blocking
    simulate_filter_data.word1 = 0x00;
    simulate_filter_data.word2 = 0xFFFFFFFF;

    m_shape->setSimulationFilterData(simulate_filter_data);

    auto query_filter_data = m_shape->getQueryFilterData();
    query_filter_data.word0 = numeric;
    m_shape->setQueryFilterData(query_filter_data);
}

CollisionGroup ShapeImpl::GetCollisionGroup() const {
    return static_cast<CollisionGroup>(
        m_shape->getSimulationFilterData().word0);
}

void ShapeImpl::EnableGenerateHitEvent(bool enable) {
    auto filter = m_shape->getSimulationFilterData();
    uint32_t bit = (1 << SimulationBehavior_Notify)
                   << SimulationBehaviorBitOffset;

    if (enable) {
        filter.word0 |= bit;
    } else {
        filter.word0 &= ~bit;
    }
    m_shape->setSimulationFilterData(filter);
}

bool ShapeImpl::IsEnabledGenerateHitEvent() const {
    return (m_shape->getSimulationFilterData().word0 >>
            SimulationBehaviorBitOffset) &
           (1 << SimulationBehavior_Notify);
}

void ShapeImpl::EnableSceneQuery(bool enable) {
    m_shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, enable);
}

bool ShapeImpl::IsEnableSceneQuery() const {
    return m_shape->getFlags() & physx::PxShapeFlag::eSCENE_QUERY_SHAPE;
}

void ShapeImpl::EnableSimulate(bool enable) {
    m_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, enable);
}

bool ShapeImpl::IsEnableSimulate() const {
    return m_shape->getFlags() & physx::PxShapeFlag::eSIMULATION_SHAPE;
}

void ShapeImpl::SetTrigger(bool enable) {
    m_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, enable);
}

bool ShapeImpl::IsTrigger() const {
    return m_shape->getFlags() & physx::PxShapeFlag::eTRIGGER_SHAPE;
}

void ShapeImpl::SetSimulateBehaviorOverlap(CollisionGroup group, bool enable) {
    auto filter_data = m_shape->getSimulationFilterData();
    uint32_t bit =
        1 << static_cast<std::underlying_type_t<CollisionGroup>>(group);
    if (enable) {
        filter_data.word1 |= bit;
    } else {
        filter_data.word1 &= ~bit;
    }
    m_shape->setSimulationFilterData(filter_data);
}

bool ShapeImpl::IsSimulateBehaviorOverlap(CollisionGroup group) const {
    auto filter_data = m_shape->getSimulationFilterData();
    return filter_data.word1 &
           1 << static_cast<std::underlying_type_t<CollisionGroup>>(group);
}

void ShapeImpl::SetSimulateBehaviorBlock(CollisionGroup group, bool enable) {
    auto filter_data = m_shape->getSimulationFilterData();
    uint32_t bit =
        1 << static_cast<std::underlying_type_t<CollisionGroup>>(group);
    if (enable) {
        filter_data.word2 |= bit;
    } else {
        filter_data.word2 &= ~bit;
    }
    m_shape->setSimulationFilterData(filter_data);
}

bool ShapeImpl::IsSimulateBehaviorBlock(CollisionGroup group) const {
    auto filter_data = m_shape->getSimulationFilterData();
    return filter_data.word2 &
           1 << static_cast<std::underlying_type_t<CollisionGroup>>(group);
}

void ShapeImpl::SetSimulateBehaviorNoCollide(CollisionGroup group) {
    uint32_t bit_reverse = ~(1 << static_cast<uint32_t>(group));
    auto filter_data = m_shape->getSimulationFilterData();
    filter_data.word1 &= bit_reverse;
    filter_data.word2 &= bit_reverse;
    m_shape->setSimulationFilterData(filter_data);
}

bool ShapeImpl::IsSimulateBehaviorNoCollide(CollisionGroup group) const {
    uint32_t bit = 1 << static_cast<uint32_t>(group);
    auto filter = m_shape->getSimulationFilterData();
    return filter.word1 & bit && filter.word2 & bit;
}

void ShapeImpl::DecRefcount() {
    RefCountable::DecRefcount();
    if (Refcount() == 0) {
        m_ctx->m_shape_allocator.MarkAsGarbage(this);
    }
}

ShapeConstImpl::ShapeConstImpl(ContextImpl* ctx, const physx::PxShape* shape)
    : ShapeImpl{ctx, const_cast<physx::PxShape*>(shape)} {}

void ShapeConstImpl::DecRefcount() {
    if (m_shape) {
        m_ctx->m_shape_const_allocator.MarkAsGarbage(this);
    }
}

}  // namespace nickel::physics