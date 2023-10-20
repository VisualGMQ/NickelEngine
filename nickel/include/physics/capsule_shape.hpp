#pragma once

#include "geom/geom2d.hpp"
#include "physics/config.hpp"
#include "physics/shape.hpp"

namespace nickel {

namespace physics {

class CapsuleShape : public Shape {
public:
    static CapsuleShape FromCapsule(const geom2d::Capsule<Real>& capsule) {
        return {capsule};
    }

    geom2d::Capsule<Real> shape;
private:

    CapsuleShape(const geom2d::Capsule<Real>& capsule) : Shape{Type::Capsule}, shape{capsule} {}
};



}

}