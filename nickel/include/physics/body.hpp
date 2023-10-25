#pragma once

#include "physics/config.hpp"

namespace nickel {

namespace physics {

/**
 * @brief component, physics body(particle)
 */
struct Body final {
    enum class Type {
        Static = 0,  // can't move, mass = 0
        Dynamic,     // can apply force, mass != 0
        Kinematic,   // can apply velocity, mass = 0
    } type;

    Vec2 pos;      // position
    Vec2 vel;      // linear velocity
    Vec2 acc;      // linear acceleration
    Vec2 force;
    Real massInv = 1.0;  // 1.0 / mass

    static Body CreateStatic(const Vec2& pos) {
        return {Type::Static, pos, {}, {}, {}, 0};
    }

    static Body CreateDynamic(const Vec2& pos, Real mass = 1.0) {
        Assert(mass != 0, "dynamic body can't set 0 mass");
        return {Type::Dynamic, pos, {}, {}, {}, static_cast<Real>(1.0 / mass)};
    }

    static Body CreateKinematic(const Vec2& pos) {
        return {Type::Dynamic, pos, {}, {}, {}, 0};
    }
};

}  // namespace physics

}  // namespace nickel