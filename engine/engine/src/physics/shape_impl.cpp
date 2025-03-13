#include "nickel/physics/internal/shape_impl.hpp"

#include "nickel/physics/internal/context_impl.hpp"
#include "nickel/physics/internal/geometry_converter.hpp"
#include "nickel/physics/internal/material_impl.hpp"
#include "nickel/physics/internal/util.hpp"

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

void ShapeImpl::SetQueryFilterData(const FilterData& filter) {
    m_shape->setQueryFilterData(FilterData2PhysX(filter));
}

void ShapeImpl::SetSimulateFilterData(const FilterData& filter) {
    m_shape->setSimulationFilterData(FilterData2PhysX(filter));
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
        auto refcount = m_shape->getReferenceCount();
        m_shape->release();
        if (refcount == 1) {
            m_shape = nullptr;
            m_ctx->m_shape_const_allocator.MarkAsGarbage(this);
        }
    }
}

}  // namespace nickel::physics