#include "nickel/physics/internal/shape_impl.hpp"

#include "nickel/physics/internal/context_impl.hpp"
#include "nickel/physics/internal/geometry_converter.hpp"
#include "nickel/physics/internal/material_impl.hpp"

namespace nickel::physics {

ShapeImpl::ShapeImpl(ContextImpl* ctx, physx::PxShape* shape)
    : m_shape{shape}, m_ctx{ctx} {}

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
    switch (g.GetType()) {
        case Geometry::Type::Box:
            m_shape->setGeometry(
                Geometry2PhysX(static_cast<const BoxGeometry&>(g)));
            break;
        case Geometry::Type::Sphere:
            m_shape->setGeometry(
                Geometry2PhysX(static_cast<const SphereGeometry&>(g)));
            break;
        case Geometry::Type::Capsule:
            m_shape->setGeometry(
                Geometry2PhysX(static_cast<const CapsuleGeometry&>(g)));
            break;
        case Geometry::Type::TriangleMesh:
            m_shape->setGeometry(
                Geometry2PhysX(static_cast<const TriangleMeshGeometry&>(g)));
            break;
        case Geometry::Type::Plane:
            m_shape->setGeometry(
                Geometry2PhysX(static_cast<const PlaneGeometry&>(g)));
            break;
    }
}

void ShapeImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_ctx->m_shape_allocator.MarkAsGarbage(this);
    }
}

}  // namespace nickel::physics