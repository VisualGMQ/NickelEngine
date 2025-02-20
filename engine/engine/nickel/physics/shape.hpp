#pragma once
#include "geometry.hpp"
#include "nickel/common/transform.hpp"
#include "nickel/physics/material.hpp"

namespace nickel::physics {
class ShapeImpl;
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

    ShapeImpl* m_impl{};
};

}