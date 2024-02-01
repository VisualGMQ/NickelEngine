#include "system/physics.hpp"

namespace nickel::physics {

void PhysicsInit(gecs::commands cmds) {
    cmds.emplace_resource<World>();
}

void PhysicsUpdate(gecs::resource<gecs::mut<World>> world,
                   gecs::querier<gecs::mut<Body>, CollideShape> querier) {
    for (auto&& [_, body, shape] : querier) {
        for (auto& forceGen : world->forceGenerators) {
            if (body.type == Body::Type::Dynamic) {
                forceGen(body);
            }
        }
    }

    world->Step(0.3, querier);
}



}