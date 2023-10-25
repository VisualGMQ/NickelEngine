#include "physics/manifold.hpp"
#include "core/cgmath.hpp"
#include "geom/basic_geom.hpp"
#include "physics/circle_shape.hpp"
#include "physics/obb_shape.hpp"
#include "physics/world.hpp"

namespace nickel {

namespace physics {

void CirclesContact::Evaluate(const CollideShape& shape1,
                              const CollideShape& shape2,
                              const Transform& trans1,
                              const Transform& trans2) {
    Assert(shape1.shape->GetType() == Shape::Type::Circle &&
               shape2.shape->GetType() == Shape::Type::Circle,
           "evaluate circles contact need shapes are both circles");

    shape1_ = &shape1;
    shape2_ = &shape2;

    auto& c1 = shape_cast<const CircleShape&>(*shape1.shape).shape;
    auto& c2 = shape_cast<const CircleShape&>(*shape2.shape).shape;

    auto center1 = c1.center + trans1.translation;
    auto center2 = c2.center + trans2.translation;
    auto v = center1 - center2;
    auto lenSqrd = v.LengthSqrd();

    if (lenSqrd >= (c1.radius + c2.radius) * (c1.radius + c2.radius)) {
        manifold_.pointCount = 0;
    } else {
        manifold_.type = Manifold::Type::Circles;
        manifold_.pointCount = 1;
        manifold_.points[0] = center1;

        if (lenSqrd == 0) {
            manifold_.normal = Vec2{1, 0};
            manifold_.depth = c1.radius;
        } else {
            auto len = std::sqrt(lenSqrd);
            manifold_.normal = v / len;
            manifold_.depth = (c1.radius + c2.radius - len) * 0.5;
        }
        manifold_.tangent = cgmath::PerpendicVec(manifold_.normal);
    }
}

void CircleAABBContact::Evaluate(const CollideShape& shape1,
                                 const CollideShape& shape2,
                                 const Transform& trans1,
                                 const Transform& trans2) {
    Assert(shape1.shape->GetType() == Shape::Type::Circle &&
               shape2.shape->GetType() == Shape::Type::OBB,
           "evaluate circles contact need shapes are both circles");

    shape1_ = &shape1;
    shape2_ = &shape2;

    auto& c = shape_cast<const CircleShape&>(*shape1.shape).shape;
    auto& aabb = shape_cast<const OBBShape&>(*shape1.shape).shape;
    auto cirCenter = c.center + trans1.translation;

    Assert(aabb.GetRotation() == 0, "currently we only support AABB");

    auto p = geom2d::AABBEdgeNearestPt(
        geom2d::AABB<Real>::FromCenter(aabb.center + trans2.translation, aabb.halfLen), cirCenter);

    auto v = cirCenter - p;
    auto len = v.Length();

    if (len > c.radius) {
        manifold_.type = Manifold::Type::FaceA;
        manifold_.pointCount = 1;
        manifold_.points[0] = p;
        manifold_.depth = c.radius - len;
        manifold_.normal = v / len;
        manifold_.tangent = cgmath::PerpendicVec(manifold_.normal);
    } else {
        manifold_.pointCount = 0;
    }
}

}  // namespace physics

}  // namespace nickel