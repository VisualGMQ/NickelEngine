#pragma once
#include "nickel/physics/filter.hpp"
#include "nickel/physics/geometry.hpp"
#include "nickel/common/transform.hpp"
#include "nickel/physics/material.hpp"

namespace nickel::physics {
class ShapeImpl;
class ShapeConstImpl;
class ContextImpl;

class Shape {
public:
    Shape() = default;
    Shape(ShapeImpl* impl);
    Shape(const Shape& o);
    Shape(Shape&& o) noexcept;
    Shape& operator=(const Shape& other);
    Shape& operator=(Shape&& other) noexcept;
    ~Shape();

    const ShapeImpl* GetImpl() const;
    ShapeImpl* GetImpl();

    void SetMaterials(std::span<Material> materials);
    void SetMaterial(Material& materials);
    
    void SetQueryFilterData(const FilterData&);
    void SetSimulateFilterData(const FilterData&);

    void SetLocalPose(const Vec3&, const Quat&);
    Transform GetLocalPose() const;

    void SetGeometry(const Geometry&);

private:
    ShapeImpl* m_impl{};
};

class ShapeConst {
public:
    ShapeConst() = default;
    ShapeConst(ShapeConstImpl* impl);
    ShapeConst(const ShapeConst& o);
    ShapeConst(ShapeConst&& o) noexcept;
    ShapeConst& operator=(const ShapeConst& other);
    ShapeConst& operator=(ShapeConst&& other) noexcept;
    ~ShapeConst();
    
    const ShapeConstImpl* GetImpl() const;
    ShapeConstImpl* GetImpl();
    
    Transform GetLocalPose() const;

private:
    ShapeConstImpl* m_impl{};
};

}  // namespace nickel::physics