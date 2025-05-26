#pragma once
#include "nickel/ecs/entity.hpp"

namespace nickel::ecs {

constexpr EntityUnderlyingType EntityGetID(Entity e) noexcept {
    return static_cast<EntityUnderlyingType>(e) & EntityIDMask;
}

constexpr EntityUnderlyingType EntityGetVersion(Entity e) noexcept {
    return static_cast<EntityUnderlyingType>(e) & EntityVersionMask;
}

constexpr bool EntityIsAlive(Entity e) noexcept {
    return static_cast<EntityUnderlyingType>(e) & EntityAliveMask;
}

constexpr Entity CreateEntity(bool is_alive, EntityUnderlyingType id,
                              EntityUnderlyingType version) {
    return static_cast<Entity>(EntityAliveMask.GenerateByValue(is_alive) |
                               EntityIDMask.GenerateByValue(id) |
                               EntityVersionMask.GenerateByValue(version));
}

}  // namespace nickel::ecs