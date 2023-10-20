#pragma once

#include "geom/geom2d.hpp"
#include "physics/config.hpp"
#include "physics/shape.hpp"


namespace nickel {

namespace physics {

class CircleShape : public Shape {
public:
    static CircleShape FromCircle(const geom2d::Circle<Real>& circle) {
        return {circle};
    }

    static CircleShape FromCenter(const Vec2& center, Real radius) {
        return {
            geom2d::Circle<Real>::Create(center, radius)
        };
    }

    geom2d::Circle<Real> shape;

private:
    CircleShape(const geom2d::Circle<Real>& circle) : Shape{Type::Circle}, shape{circle} {}
};

}  // namespace physics

}  // namespace nickel