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
                              Body* b1, Body* b2) {
    Assert(shape1.shape->GetType() == Shape::Type::Circle &&
               shape2.shape->GetType() == Shape::Type::Circle,
           "evaluate circles contact need shapes are both circles");

    init(shape1, shape2, b1, b2);

    auto& c1 = shape_cast<const CircleShape&>(*shape1.shape).shape;
    auto& c2 = shape_cast<const CircleShape&>(*shape2.shape).shape;

    auto center1 = b1 ? c1.center + b1->pos : c1.center;
    auto center2 = b2 ? c2.center + b2->pos : c2.center;
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
                                 Body* b1, Body* b2) {
    Assert((shape1.shape->GetType() == Shape::Type::Circle &&
            shape2.shape->GetType() == Shape::Type::OBB) ||
           (shape1.shape->GetType() == Shape::Type::OBB &&
            shape2.shape->GetType() == Shape::Type::Circle),
           "evaluate circles contact need shapes are both circles");

    if (shape1.shape->GetType() == Shape::Type::OBB && shape2.shape->GetType() == Shape::Type::Circle) {
        Evaluate(shape2, shape1, b2, b1);
        return;
    }

    init(shape1, shape2, b1, b2);

    auto& c = shape_cast<const CircleShape&>(*shape1.shape).shape;
    auto& obb = shape_cast<const OBBShape&>(*shape2.shape).shape;
    auto cirCenter = b1 ? c.center + b1->pos: c.center;

    Assert(obb.GetRotation() == 0, "currently we only support AABB");

    auto aabb = geom2d::AABB<Real>::FromCenter(
        b2 ? obb.center + b2->pos : obb.center, obb.halfLen);
    auto p = geom2d::AABBEdgeNearestPt(aabb, cirCenter);

    auto v = cirCenter - p;
    auto len = v.Length();

    bool inner = geom::IsAABBContain(aabb, cirCenter);

    if (len < c.radius) {
        manifold_.type = Manifold::Type::FaceA;
        manifold_.pointCount = 1;
        manifold_.points[0] = p;
        manifold_.depth = inner ? (p - cirCenter).Length() + c.radius : c.radius - len;
        manifold_.normal = (inner ? -v : v) / len;
        manifold_.tangent = cgmath::PerpendicVec(manifold_.normal);
    } else {
        manifold_.pointCount = 0;
    }
}

void Contact::ApplyImpulse() {
    constexpr float bias = 0.25;

    // auto j1 = manifold_.normal;
    // auto j2 = -manifold_.normal;
    // auto jv = j1.Dot(b1_->vel) + j2.Dot(b2_->vel);

    auto dv = b1_->vel - b2_->vel;
    float vn = Dot(dv, manifold_.normal);

    // auto jm1 = j1 * b1_->massInv;
    // auto jm2 = j2 * b2_->massInv;

    float dLambda = (-vn + bias) / (b1_->massInv + b2_->massInv);

    auto oldLambda = lambda_;
    // auto dLambda = -(jv - bias) / (b1_->massInv + b2_->massInv);
    lambda_ = std::max<float>(lambda_ + dLambda, 0);
    dLambda = lambda_ - oldLambda;
    // dLambda = std::max(dLambda, 0.0f);

    auto impuls = dLambda * manifold_.normal;

    if (b1_->type == Body::Type::Dynamic) {
        b1_->vel += impuls * b1_->massInv;
    }
    if (b2_->type == Body::Type::Dynamic) {
        b2_->vel -= impuls * b2_->massInv;
    }
}

}  // namespace physics

}  // namespace nickel