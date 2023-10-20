#pragma once

#include "config/config.hpp"
#include "core/cgmath.hpp"

namespace nickel {

namespace physics {

using Vec2 = cgmath::Vec<config::PhysicsFloatingType, 2>;
using Real = config::PhysicsFloatingType;

}

}