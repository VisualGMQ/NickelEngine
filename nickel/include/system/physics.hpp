#pragma once

#include "physics/world.hpp"
#include "common/ecs.hpp"

namespace nickel::physics {

void PhysicsInit(gecs::commands cmds);

void PhysicsUpdate(gecs::resource<gecs::mut<World>> world,
                   gecs::querier<gecs::mut<Body>, CollideShape> querier);

}