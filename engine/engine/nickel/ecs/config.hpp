#pragma once
#include "nickel/ecs/mask.hpp"

namespace nickel::ecs {

using EntityUnderlyingType = uint32_t;

constexpr Mask<EntityUnderlyingType> EntityIDMask =
    GenerateMask<EntityUnderlyingType>(16, 0);
constexpr Mask<EntityUnderlyingType> EntityVersionMask =
    GenerateMask<EntityUnderlyingType>(8, 16);
constexpr Mask<EntityUnderlyingType> EntityAliveMask =
    GenerateMask<EntityUnderlyingType>(1, 31);

using ComponentUnderlyingType = uint32_t;

constexpr Mask<ComponentUnderlyingType> ComponentIDMask =
    GenerateMask<ComponentUnderlyingType>(31, 0);
constexpr Mask<ComponentUnderlyingType> ComponentAliveMask =
    GenerateMask<ComponentUnderlyingType>(1, 31);

}  // namespace nickel::ecs