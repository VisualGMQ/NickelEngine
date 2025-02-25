#include "nickel/physics/shape.hpp"

#include "nickel/physics/internal/geometry_converter.hpp"
#include "nickel/physics/internal/shape_impl.hpp"

namespace nickel::physics {

void Shape::SetMaterials(std::span<Material> materials) {
    m_impl->SetMaterials(materials);
}

void Shape::SetMaterial(Material& mtl) {
    SetMaterials(std::span{&mtl, 1});
}

void Shape::SetLocalPose(const Vec3& p, const Quat& q) {
    return m_impl->SetLocalPose(p, q);
}

Transform Shape::GetLocalPose() const {
    return m_impl->GetLocalPose();
}

void Shape::SetGeometry(const Geometry& g) {
    m_impl->SetGeometry(g);
}

Transform ShapeConst::GetLocalPose() const {
    return m_impl->GetLocalPose();
}

}  // namespace nickel::physics