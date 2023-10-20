#include "physics/world.hpp"
#include "gecs/entity/querier.hpp"

namespace nickel {

namespace physics {

void World::Step(Real interval, Body& body) {
    physicSolver_.Step(interval, body);
}

void PhysicsInit(gecs::commands cmds) {
    cmds.emplace_resource<World>();
}

void PhysicsUpdate(gecs::resource<gecs::mut<World>> world,
                   gecs::querier<gecs::mut<Body>> bodies) {
    for (auto&& [_, body] : bodies) {
        for (auto& forceGen : world->forceGenerators) {
            forceGen(body);
        }
        world->Step(0.3, body);
    }
}

}  // namespace physics

}  // namespace nickel