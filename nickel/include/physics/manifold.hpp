#pragma once

#include "physics/config.hpp"
#include "physics/shape.hpp"

namespace nickel {

namespace physics {

struct Manifold final {
    static constexpr int MaxPointNum = 2;

    enum class Type {
        Circles,
        FaceA,
        FaceB,
    } type;

    Vec2 points[MaxPointNum];
    int pointCount = 0;
    Vec2 normal;
    Vec2 tangent;
    Real depth; // intersect depth
};

class Contact {
public:
    virtual ~Contact() = default;
    
    virtual void Evaluate(const CollideShape& shape1, const CollideShape& shape2, const Transform&, const Transform&) = 0;

    auto& GetManifold() const { return manifold_; }

protected:
    Manifold manifold_;
    const CollideShape* shape1_ = nullptr;
    const CollideShape* shape2_ = nullptr;
};

class CirclesContact: public Contact {
public:
    void Evaluate(const CollideShape& shape1, const CollideShape& shape2, const Transform&, const Transform&) override;
};

// replace this with CirclePolygonContact
class CircleAABBContact: public Contact {
public:
    void Evaluate(const CollideShape& shape1, const CollideShape& shape2, const Transform&, const Transform&) override;
};

}  // namespace physics

}  // namespace nickel