#pragma once
#include "nickel/misc/gameobject.hpp"
#include "nickel/physics/vehicle.hpp"

void ShowVehicleDebugPanel(
    nickel::GameObject& go, nickel::physics::VehicleWheelSimDescriptor&,
    nickel::physics::VehicleDriveSim4WDescriptor&);