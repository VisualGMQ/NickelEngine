#pragma once

#include "geom/geom2d.hpp"
#include "physics/config.hpp"
#include "physics/shape.hpp"

namespace nickel {

namespace physics {

class OBBShape : public Shape {
public:
    static OBBShape FromOBB(const geom2d::OBB<Real>& obb) { return {obb}; }

    static OBBShape FromCenter(const Vec2& center, const Vec2& halfLen,
                               Real rotation) {
        return {
            geom2d::OBB<Real>::FromCenter(center, halfLen, rotation)
        };
    }

    geom2d::OBB<Real> shape;

private:
    OBBShape(const geom2d::OBB<Real>& obb) : Shape(Type::OBB), shape(obb) {}
};

}  // namespace physics

}  // namespace nickel