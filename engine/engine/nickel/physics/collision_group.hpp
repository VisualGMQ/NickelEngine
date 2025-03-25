#pragma once

namespace nickel::physics {

enum class CollisionGroup : uint8_t {
    WorldStatic,
    WorldDynamic,
    VehicleChassis,
    VehicleWheel,
    CCT,

    // preserve collision groups
};

constexpr uint8_t MaxCollisionGroupCount = 32;

enum SimulationBehavior : uint8_t {
    SimulationBehavior_Notify = 0,
    SimulationBehavior_Supress,
    SimulationBehavior_CCD,
    SimulationBehavior_ModifyContacts,
};

constexpr uint8_t CollisionGroupContainBits = 5;
constexpr uint8_t CollisionGroupBitOffset = 0;
constexpr uint8_t SimulationBehaviorContainBits = 8;
constexpr uint8_t SimulationBehaviorBitOffset = CollisionGroupBitOffset;

}  // namespace nickel::physics