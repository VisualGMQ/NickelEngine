#pragma once

#include "core/assert.hpp"
#include "pch.hpp"
#include "physics/manifold_solver.hpp"
#include "physics/physic_solver.hpp"


namespace nickel {

namespace physics {

class World final {
public:
    using ForceGenerator = std::function<void(Body&)>;

    void Step(Real interval, Body&);
    std::vector<ForceGenerator> forceGenerators;

    Real MaxSpeed() const { return physicSolver_.MaxSpeed(); }
    void SetMaxSpeed(Real s) { return physicSolver_.SetMaxSpeed(s); }

private:
    PhysicSolver physicSolver_;
};

void PhysicsInit(gecs::commands cmds);

void PhysicsUpdate(gecs::resource<gecs::mut<World>> world,
                   gecs::querier<gecs::mut<Body>> bodies);

template <typename T>
T shape_cast(const Shape& s) {
    using type = std::decay_t<T>;
    switch (s.GetType()) {
        case Shape::Type::Circle:
            Assert((std::is_same_v<type, CircleShape>),
                   "cast shape to circle failed");
            break;
        case Shape::Type::OBB:
            Assert((std::is_same_v<type, OBBShape>),
                   "cast shape to circle failed");
            break;
        case Shape::Type::Polygon:
            Assert((std::is_same_v<type, PolygonShape>),
                   "cast shape to circle failed");
            break;
        case Shape::Type::Capsule:
            Assert((std::is_same_v<type, CapsuleShape>),
                   "cast shape to circle failed");
            break;
        default:
            Assert(false, "cast shape to no physics shape is invalid");
    }

    return static_cast<T>(s);
}

}  // namespace physics

}  // namespace nickel