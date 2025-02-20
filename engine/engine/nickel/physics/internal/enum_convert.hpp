#pragma once

#include "nickel/common/assert.hpp"
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

#undef CASE

}  // namespace nickel::physics