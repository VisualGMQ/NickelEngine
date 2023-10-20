#pragma once

#include "physics/body.hpp"

namespace nickel {

namespace physics {

class PhysicSolver final {
public:
    PhysicSolver(Real maxSpeed = 100): maxSpeed_{maxSpeed} {}

    void Step(Real interval, Body& body);
    Real MaxSpeed() const { return maxSpeed_; }
    void SetMaxSpeed(Real s) { maxSpeed_ = s; }

private:
    Real maxSpeed_;
};

}

}