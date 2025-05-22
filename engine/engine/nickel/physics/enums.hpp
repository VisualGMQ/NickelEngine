#pragma once
#include "nickel/common/refl_macro.hpp"

namespace nickel::physics {

enum class NICKEL_REFL_ATTR(refl) ForceMode {
    Force,
    Acceleration,
    Velocity,
    Impulse,
};

enum class NICKEL_REFL_ATTR(refl) CombineMode { Average, Min, Max, Multiply };

enum class NICKEL_REFL_ATTR(refl) RigidActorType {
    RigidStatic,
    RigidDynamic,
    ArticulationLink,
    DeformableSurface,
    DeformableVolume,
    PbdParticleSystem,
};

enum class NICKEL_REFL_ATTR(refl) HitFlag {
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

enum class NICKEL_REFL_ATTR(refl) QueryFlag {
    Static = 1 << 0,
    Dynamic = 1 << 1,
    PreFilter = 1 << 2,
    PostFilter = 1 << 3,
    AnyHit = 1 << 4,
    NoBlock = 1 << 5,
    BatchQueryLegacyBehaviour = 1 << 6,
    DisableHardcodedFilter = 1 << 7,
};

enum class NICKEL_REFL_ATTR(refl) QueryHitType {
    None,
    Touch,
    Block,
};

}  // namespace nickel::physics