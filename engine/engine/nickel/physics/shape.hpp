#pragma once
#include "geometry.hpp"
#include "nickel/common/transform.hpp"
#include "nickel/physics/material.hpp"

namespace nickel::physics {
class ShapeImpl;
class ShapeImplConst;
class ContextImpl;

class Shape {
public:
    Shape() = default;
    Shape(ShapeImpl*);
    Shape(const Shape&);
    Shape(Shape&&) noexcept;
    Shape& operator=(const Shape&);
    Shape& operator=(Shape&&) noexcept;
    ~Shape();

    void SetMaterials(std::span<Material> materials);
    void SetMaterial(Material& materials);

    void SetLocalPose(const Vec3&, const Quat&);
    Transform GetLocalPose() const;

    void SetGeometry(const Geometry&);

    ShapeImpl* GetImpl();
    const ShapeImpl* GetImpl() const;

private:
    ShapeImpl* m_impl{};
};

class ShapeConst {
public:
    ShapeConst(ShapeImplConst*);
    ShapeConst() = default;
    ShapeConst(const ShapeConst&);
    ShapeConst(ShapeConst&&) noexcept;
    ShapeConst& operator=(const ShapeConst&);
    ShapeConst& operator=(ShapeConst&&) noexcept;
    ~ShapeConst();

    Transform GetLocalPose() const;

    ShapeImplConst* GetImpl();
    const ShapeImplConst* GetImpl() const;

private:
    ShapeImplConst* m_impl{};
};

}  // namespace nickel::physics