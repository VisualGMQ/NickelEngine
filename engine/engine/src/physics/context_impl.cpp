#include "nickel/physics/internal/context_impl.hpp"
#include "nickel/physics/internal/pch.hpp"

namespace nickel::physics {

ContextImpl::ContextImpl() {
    m_foundation =
        PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_error_callback);
    if (!m_foundation) {
        LOGC("Failed to init PhysX");
    }

    m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation,
                                physx::PxTolerancesScale{});
    if (!m_physics) {
        LOGC("Failed to create PxPhysics");
    }
}

ContextImpl::~ContextImpl() {
    m_physics->release();
    m_foundation->release();
}

}  // namespace nickel::physics