#pragma once
#include "nickel/ecs/id.hpp"

namespace nickel::ecs {

constexpr IDUnderlyingType ComponentGetID(ComponentID e) noexcept {
    return static_cast<IDUnderlyingType>(e) & ComponentIDMask;
}

constexpr bool ComponentIsAlive(Entity e) noexcept {
    return static_cast<IDUnderlyingType>(e) & ComponentAliveMask;
}


}