#pragma once
#include "nickel/ecs/id.hpp"

namespace nickel::ecs {

constexpr IDUnderlyingType EntityGetID(Entity e) noexcept {
    return static_cast<IDUnderlyingType>(e) & EntityIDMask;
}

constexpr IDUnderlyingType EntityGetVersion(Entity e) noexcept {
    return static_cast<IDUnderlyingType>(e) & EntityVersionMask;
}

constexpr bool EntityIsAlive(Entity e) noexcept {
    return static_cast<IDUnderlyingType>(e) & EntityAliveMask;
}

constexpr Entity CreateEntity(bool is_alive, IDUnderlyingType id,
                              IDUnderlyingType version) {
    return static_cast<Entity>(EntityAliveMask.GenerateByValue(is_alive) |
                               EntityIDMask.GenerateByValue(id) |
                               EntityVersionMask.GenerateByValue(version));
}

constexpr Entity EntityIncreaseVersion(Entity e) noexcept {
    return CreateEntity(EntityIsAlive(e), EntityGetID(e),
                        EntityGetVersion(e) + 1);
}

}  // namespace nickel::ecs