#include "nickel/physics/shape.hpp"

#include "nickel/physics/internal/geometry_converter.hpp"
#include "nickel/physics/internal/shape_impl.hpp"

namespace nickel::physics {

Shape::Shape(ShapeImpl* impl) : m_impl{impl} {}

Shape::Shape(const Shape& o) : m_impl{o.m_impl} {
    if (o.m_impl) {
        o.m_impl->IncRefcount();
    }
}

Shape::Shape(Shape&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

Shape& Shape::operator=(const Shape& o) {
    if (&o != this) {
        if (m_impl) {
            m_impl->DecRefcount();
        }
        m_impl = o.m_impl;
        if (o.m_impl) {
            o.m_impl->IncRefcount();
        }
    }
    return *this;
}

Shape& Shape::operator=(Shape&& o) noexcept {
    if (&o != this) {
        if (m_impl) {
            m_impl->DecRefcount();
        }
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

Shape::~Shape() {
    if (m_impl) {
        m_impl->DecRefcount();
    }
}

const ShapeImpl* Shape::GetImpl() const {
    return m_impl;
}

ShapeImpl* Shape::GetImpl() {
    return m_impl;
}

void Shape::SetMaterials(std::span<Material> materials) {
    m_impl->SetMaterials(materials);
}

void Shape::SetMaterial(Material& mtl) {
    SetMaterials(std::span{&mtl, 1});
}

void Shape::SetQueryFilterData(const FilterData& filter) {
    m_impl->SetQueryFilterData(filter);
}

void Shape::SetSimulateFilterData(const FilterData& filter) {
    m_impl->SetSimulateFilterData(filter);
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

ShapeConst::ShapeConst(ShapeConstImpl* impl) : m_impl{impl} {}

ShapeConst::ShapeConst(const ShapeConst& o) : m_impl{o.m_impl} {
    if (o.m_impl) {
        o.m_impl->IncRefcount();
    }
}

ShapeConst::ShapeConst(ShapeConst&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

ShapeConst& ShapeConst::operator=(const ShapeConst& o) {
    if (&o != this) {
        if (m_impl) {
            m_impl->DecRefcount();
        }
        m_impl = o.m_impl;
        if (o.m_impl) {
            o.m_impl->IncRefcount();
        }
    }
    return *this;
}

ShapeConst& ShapeConst::operator=(ShapeConst&& o) noexcept {
    if (&o != this) {
        if (m_impl) {
            m_impl->DecRefcount();
        }
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

ShapeConst::~ShapeConst() {
    if (m_impl) {
        m_impl->DecRefcount();
    }
}

const ShapeConstImpl* ShapeConst::GetImpl() const {
    return m_impl;
}

ShapeConstImpl* ShapeConst::GetImpl() {
    return m_impl;
}

}  // namespace nickel::physics