#include "physics/physic_solver.hpp"

namespace nickel {

namespace physics {

void PhysicSolver::Step(Real interval, Body& body) {
    body.acc = body.force * interval;
    body.vel += body.acc * interval;

    if (body.vel.LengthSqrd() > maxSpeed_ * maxSpeed_) {
        body.vel = cgmath::Normalize(body.vel) * maxSpeed_;
    }

    body.pos += body.vel * interval + 0.5 * body.acc * interval * interval;

    body.force = Vec2{0, 0};
}

}

}