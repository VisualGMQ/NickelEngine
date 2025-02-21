#pragma once

namespace nickel::physics {
enum class ForceMode {
    Force,
    Acceleration,
    Velocity,
    Impulse,
};

enum class CombineMode { Average, Min, Max, Multiply };

enum class RigidActorType {
    RigidStatic,
    RigidDynamic,
    ArticulationLink,
    DeformableSurface,
    DeformableVolume,
    PbdParticleSystem,
};

enum class HitFlag {
    Position = 1 << 0,
    Normal = 1 << 1,
    UV = 1 << 2,
    AssumeNoInitialOverlap = 1 << 3,
    AnyHit = 1 << 4,
    MeshMultile = 1 << 5,
    MeshBothSides = 1 << 6,
    PreciseSweep = 1 << 7,
    MTD = 1 << 8,
    FaceIndex = 1 << 9,
    Default = Position | Normal | FaceIndex,
};
}  // namespace nickel::physics