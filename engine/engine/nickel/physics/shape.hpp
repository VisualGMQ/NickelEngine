#pragma once
#include "geometry.hpp"
#include "nickel/common/transform.hpp"
#include "nickel/physics/material.hpp"

namespace nickel::physics {
class ShapeImpl;
class ShapeImplConst;
class ContextImpl;

class Shape: public ImplWrapper<ShapeImpl> {
public:
    using ImplWrapper::ImplWrapper;
    
    void SetMaterials(std::span<Material> materials);
    void SetMaterial(Material& materials);

    void SetLocalPose(const Vec3&, const Quat&);
    Transform GetLocalPose() const;

    void SetGeometry(const Geometry&);
};

class ShapeConst: public ImplWrapper<ShapeImplConst> {
public:
    using ImplWrapper::ImplWrapper;
    
    Transform GetLocalPose() const;
};

}  // namespace nickel::physics