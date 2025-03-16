#pragma once
#include "nickel/fs/path.hpp"
#include "nickel/physics/vehicle.hpp"

void VehicleSerialize(const nickel::Path& filename,
                      const nickel::physics::VehicleWheelSimDescriptor& wheel,
                      const nickel::physics::VehicleEngineDescriptor& engine);

