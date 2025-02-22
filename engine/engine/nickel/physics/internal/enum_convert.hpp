#pragma once

#include "nickel/common/assert.hpp"
#include "nickel/common/flags.hpp"
#include "nickel/physics/enums.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/joint.hpp"

namespace nickel::physics {

#define CASE(a, b) \
    case (a):      \
        return (b);

inline physx::PxForceMode::Enum ForceMode2PhysX(ForceMode mode) {
    switch (mode) {
        CASE(ForceMode::Force, physx::PxForceMode::eFORCE)
        CASE(ForceMode::Acceleration, physx::PxForceMode::eACCELERATION)
        CASE(ForceMode::Velocity, physx::PxForceMode::eVELOCITY_CHANGE)
        CASE(ForceMode::Impulse, physx::PxForceMode::eIMPULSE)
    }

    NICKEL_CANT_REACH();
    return {};
}

inline physx::PxCombineMode::Enum CombineMode2PhysX(CombineMode mode) {
    switch (mode) {
        CASE(CombineMode::Average, physx::PxCombineMode::Enum::eAVERAGE)
        CASE(CombineMode::Min, physx::PxCombineMode::Enum::eMIN)
        CASE(CombineMode::Max, physx::PxCombineMode::Enum::eMAX)
        CASE(CombineMode::Multiply, physx::PxCombineMode::Enum::eMULTIPLY)
    }

    NICKEL_CANT_REACH();
    return {};
}

inline RigidActorType RigidActorTypeFromPhysX(physx::PxActorType::Enum type) {
    switch (type) {
        CASE(physx::PxActorType::eRIGID_STATIC, RigidActorType::RigidStatic)
        CASE(physx::PxActorType::eRIGID_DYNAMIC, RigidActorType::RigidDynamic)
        CASE(physx::PxActorType::eARTICULATION_LINK,
             RigidActorType::ArticulationLink)
        CASE(physx::PxActorType::eDEFORMABLE_SURFACE,
             RigidActorType::DeformableSurface)
        CASE(physx::PxActorType::eDEFORMABLE_VOLUME,
             RigidActorType::DeformableVolume)
        CASE(physx::PxActorType::ePBD_PARTICLESYSTEM,
             RigidActorType::PbdParticleSystem)
    }

    NICKEL_CANT_REACH();
    return {};
}

inline physx::PxD6Motion::Enum D6Motion2PhysX(D6Joint::Motion motion) {
    switch (motion) {
        CASE(D6Joint::Motion::Free, physx::PxD6Motion::eFREE);
        CASE(D6Joint::Motion::Locked, physx::PxD6Motion::eLOCKED);
        CASE(D6Joint::Motion::Limited, physx::PxD6Motion::eLIMITED);
    }

    NICKEL_CANT_REACH();
    return {};
}

inline D6Joint::Motion D6MotionFromPhysX(physx::PxD6Motion::Enum motion) {
    switch (motion) {
        CASE(physx::PxD6Motion::eFREE, D6Joint::Motion::Free);
        CASE(physx::PxD6Motion::eLOCKED, D6Joint::Motion::Locked);
        CASE(physx::PxD6Motion::eLIMITED, D6Joint::Motion::Limited);
    }

    NICKEL_CANT_REACH();
    return {};
}

#define TRY_SET_BIT(src, dst) \
    if (flags & (src)) bits |= (dst);

inline physx::PxHitFlags HitFlag2PhysX(Flags<HitFlag> flags) {
    physx::PxHitFlags bits{};
    TRY_SET_BIT(HitFlag::Normal, physx::PxHitFlag::eNORMAL);
    TRY_SET_BIT(HitFlag::Position, physx::PxHitFlag::ePOSITION);
    TRY_SET_BIT(HitFlag::AnyHit, physx::PxHitFlag::eANY_HIT);
    TRY_SET_BIT(HitFlag::FaceIndex, physx::PxHitFlag::eFACE_INDEX);
    TRY_SET_BIT(HitFlag::MeshMultile, physx::PxHitFlag::eMESH_MULTIPLE);
    TRY_SET_BIT(HitFlag::PreciseSweep, physx::PxHitFlag::ePRECISE_SWEEP);
    TRY_SET_BIT(HitFlag::UV, physx::PxHitFlag::eUV);
    TRY_SET_BIT(HitFlag::MeshBothSides, physx::PxHitFlag::eMESH_BOTH_SIDES);
    TRY_SET_BIT(HitFlag::MeshMultile, physx::PxHitFlag::eMESH_MULTIPLE);
    TRY_SET_BIT(HitFlag::MTD, physx::PxHitFlag::eMTD);
    TRY_SET_BIT(HitFlag::AssumeNoInitialOverlap,
                physx::PxHitFlag::eASSUME_NO_INITIAL_OVERLAP);
    return bits;
}

inline Flags<HitFlag> HitFlagFromPhysX(physx::PxHitFlags flags) {
    Flags<HitFlag> bits{};
    TRY_SET_BIT(physx::PxHitFlag::eNORMAL, HitFlag::Normal);
    TRY_SET_BIT(physx::PxHitFlag::ePOSITION, HitFlag::Position);
    TRY_SET_BIT(physx::PxHitFlag::eANY_HIT, HitFlag::AnyHit);
    TRY_SET_BIT(physx::PxHitFlag::eFACE_INDEX, HitFlag::FaceIndex);
    TRY_SET_BIT(physx::PxHitFlag::eMESH_MULTIPLE, HitFlag::MeshMultile);
    TRY_SET_BIT(physx::PxHitFlag::ePRECISE_SWEEP, HitFlag::PreciseSweep);
    TRY_SET_BIT(physx::PxHitFlag::eUV, HitFlag::UV);
    TRY_SET_BIT(physx::PxHitFlag::eMESH_BOTH_SIDES, HitFlag::MeshBothSides);
    TRY_SET_BIT(physx::PxHitFlag::eMESH_MULTIPLE, HitFlag::MeshMultile);
    TRY_SET_BIT(physx::PxHitFlag::eMTD, HitFlag::MTD);
    TRY_SET_BIT(physx::PxHitFlag::eASSUME_NO_INITIAL_OVERLAP,
                HitFlag::AssumeNoInitialOverlap);
    return bits;
}

inline physx::PxQueryFlags QueryFlags2PhysX(Flags<QueryFlag> flags) {
    physx::PxQueryFlags bits;
    TRY_SET_BIT(QueryFlag::Static, physx::PxQueryFlag::eSTATIC);
    TRY_SET_BIT(QueryFlag::Dynamic, physx::PxQueryFlag::eDYNAMIC);
    TRY_SET_BIT(QueryFlag::AnyHit, physx::PxQueryFlag::eANY_HIT);
    TRY_SET_BIT(QueryFlag::NoBlock, physx::PxQueryFlag::eNO_BLOCK);
    TRY_SET_BIT(QueryFlag::PostFilter, physx::PxQueryFlag::ePOSTFILTER);
    TRY_SET_BIT(QueryFlag::PreFilter, physx::PxQueryFlag::ePREFILTER);
    TRY_SET_BIT(QueryFlag::DisableHardcodedFilter,
                physx::PxQueryFlag::eDISABLE_HARDCODED_FILTER);
    TRY_SET_BIT(QueryFlag::BatchQueryLegacyBehaviour,
                physx::PxQueryFlag::eBATCH_QUERY_LEGACY_BEHAVIOUR);
    return bits;
}

inline physx::PxQueryHitType::Enum QueryHitType2PhysX(QueryHitType flags) {
    switch (flags) {
        CASE(QueryHitType::None, physx::PxQueryHitType::eNONE);
        CASE(QueryHitType::Block, physx::PxQueryHitType::eBLOCK);
        CASE(QueryHitType::Touch, physx::PxQueryHitType::eTOUCH);
    }

    NICKEL_CANT_REACH();
    return {};
}

#undef TRY_SET_BIT
#undef CASE

}  // namespace nickel::physics