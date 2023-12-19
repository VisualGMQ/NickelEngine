#pragma once

#include "physics/body.hpp"
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
    
    virtual void Evaluate(const CollideShape& shape1, const CollideShape& shape2, Body*, Body*) = 0;

    auto& GetManifold() const { return manifold_; }

    Body* GetBody1() { return b1_; }
    Body* GetBody2() { return b2_; }

protected:
    Manifold manifold_;
    const CollideShape* shape1_ = nullptr;
    const CollideShape* shape2_ = nullptr;
    Body* b1_ = nullptr;
    Body* b2_ = nullptr;

    void init(const CollideShape& shape1, const CollideShape& shape2, Body* b1, Body* b2) {
        shape1_ = &shape1;
        shape2_ = &shape2;
        b1_ = b1;
        b2_ = b2;
    }
};

class CirclesContact: public Contact {
public:
    void Evaluate(const CollideShape& shape1, const CollideShape& shape2, Body*, Body*) override;
};

// replace this with CirclePolygonContact
class CircleAABBContact: public Contact {
public:
    void Evaluate(const CollideShape& shape1, const CollideShape& shape2, Body*, Body*) override;
};

}  // namespace physics

}  // namespace nickel