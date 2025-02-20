#pragma once

namespace nickel::physics {

enum class ForceMode {
    Force,
    Acceleration,
    Velocity,
    Impulse,
};

enum class CombineMode {
    Average,
    Min,
    Max,
    Multiply
};

enum class RigidActorType {
    RigidStatic,
    RigidDynamic,
    ArticulationLink,
    DeformableSurface,
    DeformableVolume,
    PbdParticleSystem,
};

}  // namespace nickel::physics