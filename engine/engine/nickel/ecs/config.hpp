#pragma once
#include "nickel/ecs/mask.hpp"

namespace nickel::ecs {

using IDUnderlyingType = uint32_t;

constexpr auto EntityIDMask = GenerateMask<IDUnderlyingType>(16, 0);
constexpr auto EntityVersionMask = GenerateMask<IDUnderlyingType>(8, 16);
constexpr auto EntityAliveMask = GenerateMask<IDUnderlyingType>(1, 31);

constexpr auto ComponentIDMask = GenerateMask<IDUnderlyingType>(31, 0);
constexpr auto ComponentAliveMask = GenerateMask<IDUnderlyingType>(1, 31);

}  // namespace nickel::ecs